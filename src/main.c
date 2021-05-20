/* main.c
 * (c) Jonas Gunz, 2019
 * License: MIT
 * */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "log.h"
#include "server.h"

#ifdef _TEST
#include "test.h"
#endif

void print_help( char *_argv0 ) {
	printf(
		"dns\n"
		"Usage: %s [OPTIONS]\n\n"
		"Options:\n"
		"	-h		Display this help text\n"
		"	-i IP		IP to bind to, default: 0.0.0.0\n"
		"	-p PORT 	Port to listen, default: 53 on\n"
		"	-z FILE 	Zonefile\n", _argv0
	);
}

void parse_args( server_config_t *_config, int argc, char* argv[]) {
	memset( _config, 0, sizeof( server_config_t ) );

	_config->bind_ip = "0.0.0.0";
	_config->bind_port = 53;
	_config->zonefile = "/nofile";

	for( int i = 1; i < argc; i++ ) {
		const int icpy = i;
		if ( argv[i][0] != '-' ) {
			print_help( argv[0] );
			exit( 1 );
		}

		for( int o = 1; o < strlen(argv[icpy]); o++ ) {
			switch( argv[icpy][o] ) {
				case 'h':
					print_help( argv[0] );
					exit( 0 );
				case 'i':
					_config->bind_ip = argv[++i];
					break;
				case 'p':
					_config->bind_port = atoi( argv[++i] );
					break;
				case 'z':
					_config->zonefile = argv[++i];
					break;
				default:
					printf( "Unkown option \"%c\"\n", argv[icpy][o] );
					print_help( argv[0] );
					exit( 1 );
			}
		}
	}
}

int main(int argc, char* argv[])
{
	server_config_t config;
	parse_args( &config, argc, argv );

	log_init_stdout(_LOG_DEBUG);
	
#ifdef _TEST
	run_test();
#else
	server_start( &config );
#endif
	return 0;
}

