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

int main(int argc, char* argv[])
{
	log_init_stdout(_LOG_DEBUG);
	//CMD line arg parsing goes in here
#ifdef _TEST
	run_test();
#else
	server_config_t config;

	memset(&config, 0, sizeof config);

	config.bind_ip = "0.0.0.0";
	config.bind_port = 53;
	config.zonefile = "/nofile";

	server_start( &config );
#endif

}

