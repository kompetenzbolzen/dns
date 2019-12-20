/* dns.h
 * (c) Jonas Gunz, 2019
 * License: MIT
 * */

#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

//TODO remove
#include <stdio.h>

//TODO remove defines
//Resource Records
#define RR_A	1
#define RR_NS	2
#define RR_CNAME 5
#define RR_SOA	6
#define RR_MX	15
#define RR_TXT	16
#define RR_AAAA	28
#define RR_SRV	33
enum dns_record {
	A	= 1,
	NS	= 2,
	CNAME	= 5,
	SOA	= 6,
	MX	= 15,
	TXT	= 16,
	AAAA	= 28,
	SRV	= 33
};

//Record Classes
#define CL_IN	1 //Internet
#define CL_CS	2 //CSNET (Onsolete)
#define CL_CH	3 //CHAOS
#define CL_HS	4 //Hesiod
enum dns_record_class {
	IN	= 1,
	CS	= 2,
	CH	= 3,
	HS	= 4
};

//OPCODES
#define OP_Q	0 //Query
#define OP_IQ	1 //Inverse Query
#define OP_STAT	2 //Status request
enum dns_opcode {
	QUERY	= 0,
	INVERSE	= 1,
	STATUS	= 2
};

//Responsecode
#define RCODE_NOERR	0
#define RCODE_FORMAT	1
#define RCODE_SERVFAIL	2
#define RCODE_NAMEERR	3
#define RCODE_NI	4 //Not implemented
#define RCODE_REFUSED	5
enum dns_responsecode {
	NOERR	= 0,
	FORMAT	= 1,
	SERVFAIL= 2,
	NAMEERR	= 3,
	NOTIMPL	= 4,
	REFUSED	= 5
};

#define FLIP_BYTES(u)	(((0x00FF & u) << 8) | ((0xFF00 & u) >> 8))

/**
 * Data is COPIED
 * */
struct dns_header;

/**
 * QNAME is REFERENCED
 * */
struct dns_question;

/**
 * NAME is REFERENCED
 * */
struct dns_answer;

/**
 * DNS Message struct
 *
 * A initialized instance is only valid as long as
 * the buffer used to create it remains unchanged
 * */
struct dns_message;

struct dns_header {
	uint16_t id;

	uint8_t QR;	//Query:0 Reply:1
	uint8_t OPCODE;	//Query:0 Iquery:1 Status:2
	uint8_t AA;	//Authorative answer
	uint8_t TC;	//Truncation
	uint8_t RD;	//Recursion Desired
	uint8_t RA;	//Recursion Available
	uint8_t Z;	//Unused
	uint8_t RCODE;	//Response Code

	uint16_t question_count;
	uint16_t answer_count;
	uint16_t authorative_count;
	uint16_t additional_count;
};

struct dns_question {
	const char* qname;
	uint16_t qtype;
	uint16_t qclass;
};

struct dns_answer {
	char* name; //in qname format
	uint16_t type;
	uint16_t class;
	uint32_t ttl;
	uint16_t rdlength;
	char* rdata;
};

struct dns_message {
	struct dns_header header;

	int question_count;
	struct dns_question* question;

	int answer_count;
	struct dns_answer* answer;
};

int dns_construct_header (
		struct	dns_header* _header,
		char*	_buffer,
		int	_bufflen
		);

/**
 * Frees all malloced memory
 * */
int dns_destroy_struct ( struct dns_message* _msg );

/**
 * Parse the packet in _buffer and populate the dns_message struct
 * Struct may still be written to on failure but contents are invalid
 * returns: 0 on success, !=0 on failure
 *
 * ONLY WRITES QUESTION SECTION. ALL OTHER ARE IGNORED
 *
 * */
int dns_parse_packet ( char* _buffer, int _bufflen, struct dns_message* _msg );

/**
 * Convert a null terminated string containing a 
 * fqdn (eg server.example.com) to the binary format used by DNS records
 * ( [6]server[7]example[3]com[0] )
 * returns: length of string in _sink, < 0 on failure
 * _sink might not be terminated on error.
 * */
int fqdn_to_qname( char* _source, int _sourcelen, char* _sink, int _sinklen );

/**
 * Convert a QNAME back to a FQDN, reversing fqdn_to_qname( )
 * returns: length of string in _sink, < 0 on failure
 * _sink may still be altered in failed attempts, but not terminated.
 * */
int qname_to_fqdn( char* _source, int _sourcelen, char* _sink, int _sinklen );

/**
 * Check a QNAME and get length
 * accepts only ASCII alphanumeric characters
 * returns: length of QNAME including NULL-byte at the end, < 0 on error
 * */
int qname_check( char* _source, int _sourcelen );
