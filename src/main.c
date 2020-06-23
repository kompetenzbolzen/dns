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
	//CMD line arg parsing goes in here
#ifdef _TEST
	run_test();
#else
	run_dns_server();
#endif

}

