#pragma once

#include <stdint.h>

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
