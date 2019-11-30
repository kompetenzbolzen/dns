#pragma once

#include <stdint.h>

//TODO remove
#include <stdio.h>

struct dns_header;

struct dns_question;

struct dns_answer;

/**
 * DNS Message struct
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
	char* qname;
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
	
	int qcount;
	struct dns_question* question;

	int acount;
	struct dns_answer* answer;
};

/**
 * Parse the packet in _buffer and populate the dns_message struct
 * returns: 0 on success, !=0 on failure
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
