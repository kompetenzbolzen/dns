#include "dns.h"

int fqdn_to_qname( char* _source, int _sourcelen, char* _sink ,int _sinklen )
{
	int i;
	int lastdot = 0;

	if (_sourcelen < 1 || _sinklen < 1)
		return -1;

	_sink[0] = ' '; //Set to known value

	for(i = 0; ((i < _sourcelen) && (i < (_sinklen - 1))); i++) { //Copy offset 1
		if(! _source[i])
			break;
		_sink[i+1] = _source[i];
	}

	if( _source[i] ) // _source not terminated, thus no valid string
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
	if ( !_sourcelen || !_sinklen ) {
		return -1;
	}

	unsigned int next_dot = _source[0] + 1;
	int i = 1;
	for(i = 1; i < _sourcelen; i++) {
		if( i > _sinklen){ //Output too small. Not >= bc sink[i-1] is used
			return -1;
		}
		if ( !_source[i] ) {
			_sink[i-1] = '\0';
			break;
		} else if (i == next_dot) {
			_sink[i-1]='.';
			next_dot = _source[i] + i + 1;
		} else {
			_sink[i-1] = _source[i];
		}

	}

	return i-1;
}
