/* dns.c
 * (c) Jonas Gunz, 2019
 * License: MIT
 * */

#include "dns.h"

int dns_construct_header ( char* _buffer, int _bufflen, dns_header_t* _header )
{
	if ( !_buffer || !_header || _bufflen < 12 )
		return -1;

	*((uint16_t*)_buffer) = _header->id; /* Since only copied, no flipping necessary */
	_buffer[2] =
		(char)((_header->QR & 0x01) << 7) |
		(char)((_header->OPCODE & 0x0F) << 3) |
		(char)((_header->AA & 0x01) << 2) |
		(char)((_header->TC & 0x01) << 1) |
		(char)( _header->RD & 0x01);
	_buffer[3] =
		(char)((_header->RA & 0x01) << 7) |
		(char)((_header->Z  & 0x07) << 4) |
		(char)( _header->RCODE & 0x0F);
	*((uint16_t*)(_buffer + 4 )) = (uint16_t)FLIP_BYTES(_header->question_count);
	*((uint16_t*)(_buffer + 6 )) = (uint16_t)FLIP_BYTES(_header->answer_count);
	*((uint16_t*)(_buffer + 8 )) = (uint16_t)FLIP_BYTES(_header->authorative_count);
	*((uint16_t*)(_buffer + 10)) = (uint16_t)FLIP_BYTES(_header->additional_count);

	return DNS_HEADER_LEN;
}

int dns_construct_answer (
		char*	_buffer,
		int	_bufflen,
		dns_answer_t* _answer
		) {
	int ret = 0;

	if ( !_buffer || _bufflen <= 0 || !_answer )
		return -1;

	/* Check buffer size */
	if ( _answer->qname_len + _answer->rdlength + 10 > _bufflen )
		return -1;

	memcpy( _buffer, _answer->qname, (unsigned)_answer->qname_len );
	ret += _answer->qname_len;

	*((uint16_t*)(_buffer + ret + 0 )) = (uint16_t)FLIP_BYTES(_answer->type);
	*((uint16_t*)(_buffer + ret + 2 )) = (uint16_t)FLIP_BYTES(_answer->class);
	*((uint16_t*)(_buffer + ret + 4 )) = (uint16_t)FLIP_BYTES((uint16_t)((_answer->ttl << 16) & 0xffff));
	*((uint16_t*)(_buffer + ret + 6 )) = (uint16_t)FLIP_BYTES((uint16_t)(_answer->ttl & 0xffff));
	*((uint16_t*)(_buffer + ret + 8 )) = (uint16_t)FLIP_BYTES(_answer->rdlength);
	ret += 10;

	memcpy( _buffer + ret, _answer->rdata, _answer->rdlength );
	ret += _answer->rdlength;

	return ret;
}

int dns_construct_questoin (
		char*	_buffer,
		int	_bufflen,
		dns_question_t* _question
		) {
	/* TODO Test */
	int ret = 0;

	if ( !_buffer || _bufflen <= 0 || !_question )
		return -1;

	/*  Check buffer size */
	if ( _question->qname_len + 4 > _bufflen )
		return -1;

	memcpy( _buffer, _question->qname, (unsigned)_question->qname_len );
	ret += _question->qname_len;

	*((uint16_t*)(_buffer + ret + 0 )) = (uint16_t)FLIP_BYTES(_question->qtype);
	*((uint16_t*)(_buffer + ret + 2 )) = (uint16_t)FLIP_BYTES(_question->qclass);
	ret += 4;

	return ret;
}


/* Question and answer count come from header */
int dns_construct_packet (
		char*	_buffer,
		int	_bufflen,
		dns_message_t* _message
		) {
	return -1;
}


int dns_destroy_struct ( dns_message_t* _msg )
{
	if ( !_msg )
		return -1;

	if ( _msg->question_count > 0 && _msg->question) {
		free ( _msg->question );
		_msg->question = NULL;
	}

	if ( _msg->answer_count > 0 && _msg->answer) {
		free ( _msg->answer );
		_msg->answer = NULL;
	}

	return 0;
}

int dns_parse_packet ( char* _buffer, int _bufflen, dns_message_t* _msg )
{
	int i = 0;
	size_t qsize;
	int ptr;
	/* TODO refactor */

	if ( !_buffer || !_bufflen || !_msg )
		return 1; /* Invalid input */

	if ( _bufflen < 12 )
		return 1; /* Too short to contain a DNS header */

	/* TODO test */
	_msg->header.id = *( (uint16_t*) _buffer );
	_msg->header.QR =  ( 0x80 & *( (uint8_t*) (_buffer + 2)) ) >> 7;
	_msg->header.OPCODE = (0x78 & *( (uint8_t*) (_buffer + 2))) >> 3;
	_msg->header.AA = (0x04 & *( (uint8_t*) (_buffer + 2))) >> 2;
	_msg->header.TC = (0x02 & *( (uint8_t*) (_buffer + 2))) >> 1;
	_msg->header.RD = (0x01 & *( (uint8_t*) (_buffer + 2)));
	_msg->header.RA = (0x80 & *( (uint8_t*) (_buffer + 3))) >> 7;
	_msg->header.Z  = (0x70 & *( (uint8_t*) (_buffer + 3))) >> 4;
	_msg->header.RCODE = (0x0F & *( (uint8_t*) (_buffer + 3)));
	_msg->question_count = _msg->header.question_count = (uint16_t)(*((uint8_t*) (_buffer + 4 )) << 8) | *((uint8_t*) (_buffer + 5 ));
	_msg->answer_count   = _msg->header.answer_count   = (uint16_t)(*((uint8_t*) (_buffer + 6 )) << 8) | *((uint8_t*) (_buffer + 7 ));
	_msg->header.authorative_count	= (uint16_t)(*((uint8_t*) (_buffer + 8 )) << 8) | *((uint8_t*) (_buffer + 9 ));
	_msg->header.additional_count	= (uint16_t)(*((uint8_t*) (_buffer + 10)) << 8) | *((uint8_t*) (_buffer + 11));

	/* TODO remove */
	/*printf("ANSWER %i\n", _msg->header.answer_count);
	printf("QUESTI %i\n", _msg->header.question_count);
	printf("AUTHOR %i\n", _msg->header.authorative_count);
	printf("ADDITI %i\n", _msg->header.additional_count);
	*/

	/* Check for sensible QD, AN, NS and ARCOUNTS before massive memory allocation */
	if(	_msg->header.question_count > 4 ||
		_msg->header.answer_count > 32 ||
		_msg->header.authorative_count > 32 ||
		_msg->header.additional_count > 32 ) {
		return 1;
	}

	/*
	 * Allocate question array
	 * TODO Only implements question section.
	 */
	qsize = sizeof(*(_msg->question)) * (unsigned)_msg->question_count;
	_msg->question_count = _msg->header.question_count;
	_msg->question = malloc ( qsize );
	memset( _msg->question, 0, qsize );

	if (!_msg->question) /* malloc failed */
		return 1;

	ptr = 12; /* byte counter */

	/* TODO refactor */
	for ( i = 0; i < _msg->question_count; i++ ) {
		int qname_len = qname_check ( (_buffer + ptr), _bufflen - ptr);

		if (qname_len <= 0) /* Check for faulty QNAME */
			return 1;

		_msg->question[i].qname = _buffer + ptr;
		_msg->question[i].qname_len = qname_len;
		ptr += qname_len;

		if( ptr >= (_bufflen - 4) ) /* Out of bounds check */
			return 1;

		_msg->question[i].qtype = (uint16_t)((uint8_t)*(_buffer + ptr) << 8) | ((uint8_t)*(_buffer + ptr + 1));
		ptr += 2;
		_msg->question[i].qclass = (uint16_t)((uint8_t)*(_buffer + ptr) << 8) | ((uint8_t)*(_buffer + ptr + 1));
		ptr += 2;

	}

	return 0;
}

int fqdn_to_qname( char* _source, int _sourcelen, char* _sink ,int _sinklen )
{
	int i, o;
	int lastdot = 0;

	if (_sourcelen < 1 || _sinklen < 1)
		return -1;

	_sink[0] = ' '; /* Set to known value */

	for(i = 0; ((i < _sourcelen) && (i < (_sinklen - 1))); i++) { /* Copy offset 1 */
		if(! _source[i])
			break;
		_sink[i+1] = _source[i];
	}

	if( _source[i] ) {
		_sink[0] = 0; /* ensure _sink is terminated */
		return -1;
	}

	for (o = 0; o < i; o++) {
		if( _sink[o] == '.') {
			_sink[lastdot] = (char)(o - lastdot - 1);
			lastdot = o;
		}
	}

	_sink[lastdot] = (char)(i - lastdot);
	_sink[i + 1] = 0;

	return i+2;
}

int fqdn_check ( char* _source, int _sourcelen ) {
	int i;
	char c;

	if ( !_source || _sourcelen <= 0 )
		return -1;

	for( i=0; i<_sourcelen; i++ ) {
		c = _source[i];
		if (!c)
			break;
		if (
			!(c>='0' && c<='9') &&
			!(c>='A' && c<='Z') &&
			!(c>='a' && c<='z') &&
			!(c== '-') && !(c=='_') && !(c=='.')
			) {
			return i+1;
		}
	}

	if ( _source[ i-1 ] != '.' )
		return i;
	if ( _source[ 0 ] == '-' )
		return 1;

	return 0;
}

int qname_to_fqdn( char* _source, int _sourcelen, char* _sink, int _sinklen )
{
	unsigned int next_dot;
	unsigned int i = 1;

	if ( !_sourcelen || !_sinklen ) {
		return -1;
	}

	next_dot = (unsigned)_source[0] + 1;

	for(i = 1; i < (unsigned)_sourcelen; i++) {
		if( i > (unsigned)_sinklen){ /* Output too small. Not >= bc sink[i-1] is used */
			_sink[0] = 0; /* ensure _sink is terminated */
			return -1;
		}
		if ( !_source[i] ) {
			_sink[i-1] = '\0';
			break;
		} else if (i == next_dot) {
			_sink[i-1]='.';
			next_dot = (unsigned)_source[i] + i + 1;
		} else {
			_sink[i-1] = _source[i];
		}
	}

	return (signed)i-1;
}

int qname_check( char* _source, int _sourcelen )
{
	int next_dot = 0;
	int i = 0;

	if (!_sourcelen)
		return -1;

	/* TODO questionable control flow */
	for (i = 0; i < _sourcelen; i++) {
		if ( i == next_dot ) {
			if (_source[i]) { /* Not last dot */
				next_dot = _source[i] + i + 1;
			} else {  /* last dot */
				return i+1;
			}
		} else if (!_source[i]) { /* Unexpected \0 */
			return -1;
		} else if (
			!(_source[i]>='0' && _source[i]<='9') &&
			!(_source[i]>='A' && _source[i]<='Z') &&
			!(_source[i]>='a' && _source[i]<='z') &&
			!(_source[i]== '-') && !(_source[i]=='_')
			) {
			return -1;
		}
	}

	return -1;
}
