#include "dns.h"

int fqdn_to_qname( char* _source, int _sourcelen, char* _sink ,int _sinklen )
{
	// TODO Opttimize
	int i = 0;
	int lastdot = 0;

	if (_sourcelen < 1 || _sinklen < 1)
		return -1;

	_sink[0] = ' '; //Set to known value

	while ( (i < _sourcelen) && (i < (_sinklen - 1))) { //Copy offset 1
		if(! _source[i])
			break;
		_sink[i+1] = _source[i];
		i++;
	}

	if( _source[i] ) // _source not terminated
		return -1;

	for (int o = 0; o < i; o++) {
		if( _sink[o] == '.') {
			_sink[lastdot] = o - lastdot - 1;
			lastdot = o;
		}
	}

	_sink[lastdot] = i - lastdot;
	_sink[i + 1] = 0;

	return i+2; 
}

int qname_to_fqdn( char* _source, int _sourcelen, char* _sink, int _sinklen )
{
	return -1;
}
