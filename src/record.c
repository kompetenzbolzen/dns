/*
 * src/record.c
 * (c) 2021 Jonas Gunz <himself@jonasgunz.de>
 * License: MIT
 */

/* https://datatracker.ietf.org/doc/html/rfc1035#section-3.3 */

#include "record.h"

/*
 * Prototypes for rdata from string functions
 * Arguments:
 *	_str:	String representing rdata
 *	_rdata:	A buffer containing the raw rdata will be alloced here
 * Return: Length of alloced _rdata buffer, <0 on error
 */

/* Obsolete record types. Will throw error. */
static ssize_t record_rdata_obsolete(char* _str, void** _rdata);

/* Unimplemented record types. Will throw error. */
static ssize_t record_rdata_not_implemented(char* _str, void** _rdata);

/* rdata that does not need conversion from string form */
static ssize_t record_rdata_verbatim(char* _str, void** _rdata);

/* rdata from fqdn to qname */
static ssize_t record_rdata_qname(char* _str, void** _rdata);

/* IPv4 Addresses */
static ssize_t record_rdata_a(char* _str, void** _rdata);

/* Start of authority */
static ssize_t record_rdata_soa(char* _str, void** _rdata);

static const char* const record_types[] = {
	"A",
	"NS",
	"MD",
	"MF",
	"CNAME",
	"SOA",
	"MB",
	"MG",
	"MR",
	"NULL",
	"WKS",
	"PTR",
	"HINFO",
	"MINFO",
	"MX",
	"TXT"
};
static const uint16_t record_types_len = sizeof(record_types) / sizeof(char*);

static ssize_t (*record_rdata_creator[])(char*, void**) = {
	&record_rdata_a, /* A */
	&record_rdata_qname, /* NS */
	&record_rdata_obsolete, /* MD */
	&record_rdata_obsolete, /* MF */
	&record_rdata_qname, /* CNAME */
	&record_rdata_soa, /* SOA */

	&record_rdata_not_implemented,
	&record_rdata_not_implemented,
	&record_rdata_not_implemented,
	&record_rdata_not_implemented,
	&record_rdata_not_implemented,
	&record_rdata_not_implemented,
	&record_rdata_not_implemented,
	&record_rdata_not_implemented,
	&record_rdata_not_implemented,

	&record_rdata_verbatim, /* TXT */
};

static const char* const record_classes[] = {
	"IN",
	"CS",
	"CH",
	"HS"
};
static const uint16_t record_classes_len = sizeof(record_classes) / sizeof(char*);

/* Implementation of RDATA cobverters */

static ssize_t record_rdata_obsolete(char* _str, void** _rdata) {
	LOGPRINTF(_LOG_ERROR, "Record type not obsolete");
	return -1;
}

static ssize_t record_rdata_not_implemented(char* _str, void** _rdata) {
	LOGPRINTF(_LOG_ERROR, "Record type not implemented");
	return -1;
}

static ssize_t record_rdata_verbatim(char* _str, void** _rdata) {
	size_t len;

	if ( !_str || !_rdata )
		return -1;

	len = strlen(_str) + 1; /* Including \0 */

	*_rdata = malloc(len);

	if ( !*_rdata )
		return -1;

	strncpy(*_rdata, _str, len);

	return (signed) len;
}

static ssize_t record_rdata_qname(char* _str, void** _rdata) {
	size_t len;
	size_t qlen;

	if ( !_str || !_rdata )
		return -1;

	len = strlen(_str); /* Including \0 */
	qlen = len + 1;

	if (fqdn_check(_str, len) < 0) {
		LOGPRINTF(_LOG_ERROR, "Not a valid FQDN");
		return -1;
	}

	/*
	 * TODO Some funny business is going on around here
	 * Related to fqdn_to_qname()
	 */
	*_rdata = malloc(qlen+1);

	if ( !*_rdata )
		return -1;

	/*strncpy(*_rdata, _str, len);*/
	fqdn_to_qname(_str, len, *_rdata, qlen);

	return (signed) qlen;
}

static ssize_t record_rdata_a(char* _str, void** _rdata) {
	char* tok;
	char* str;
	char* end;
	size_t len;
	int i;

	if ( !_str || !_rdata )
		return -1;

	len = strlen(_str) + 1;
	str = malloc(len);
	if ( !str )
		return -1;
	strncpy(str, _str, len);

	*_rdata = malloc(4);
	if ( !*_rdata )
		goto err;

	tok = strtok(str, ".");
	if( !tok )
		goto err;

	((uint8_t*)(*_rdata))[0] = (uint8_t) strtol(tok, &end, 10);

	if( *end != '\0' )
		goto err;

	for( i=1; i<4; i++) {
		tok = strtok(NULL, ".");

		if( !tok )
			goto err;

		((uint8_t*)(*_rdata))[i] = (uint8_t) strtol(tok, &end, 10);

		if( *end != '\0' )
			goto err;
	}

	free(str);
	return 4;
err:
	free(str);
	return -1;
}

static ssize_t record_rdata_soa(char* _str, void** _rdata) {
	LOGPRINTF(_LOG_ERROR, "Record type not implemented");
	return -1;
}


/* Other methods */

static uint16_t record_match_from_array(char* _str, const char* const _arr[], uint16_t _len) {
	uint16_t i;

	for( i=0; i<_len; i++ ){
		if ( strcasecmp( _str, _arr[i] ) == 0 )
			return i+1; /* Indices start with 1 */
	}

	return 0;
}

uint16_t record_class_from_str(char* _str) {
	return record_match_from_array(_str, record_classes, record_classes_len);
}

uint16_t record_type_from_str(char* _str) {
	return record_match_from_array(_str, record_types, record_types_len);
}

ssize_t record_rdata_from_str(void** _rdata, char *_str, uint16_t _rdtype) {
	uint16_t index;

	if ( _rdtype > record_types_len )
		return -1;

	if ( !_rdata || !_str )
		return -1;

	index = _rdtype - 1;

	return (*record_rdata_creator[index])(_str, _rdata);
}
