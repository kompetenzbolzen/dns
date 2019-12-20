/* main.c
 * (c) Jonas Gunz, 2019
 * License: MIT
 * */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#include <unistd.h>

#include <signal.h>

#include "dns.h"
#include "log.h"

#include "zonefile.h"

#define PRINT_ERRNO() {printf("%s:%i %i:%s\n", __FILE__, __LINE__, errno, strerror(errno));}

#define UDP_BUFFER_LEN 512

//main server socket
int sock_server;

int handle_connection (	int _socket,
			struct sockaddr_in *sockaddr_client,
			socklen_t sockaddr_client_len,
			char* _buffer,
			int _bufflen );

void signal_term ( );

void signal_term_child ( );

void signal_child ( );

int main1(	int argc,
		char* argv[] )
{
	printf("TEST MODE. NOT FUNCTIONAL\n");

	printf ("%i\n", string_compare (argv[1], argv[2]));

	return 0;
	/*
	//Fuzztest the QNAME checker
	FILE* urand = fopen ("/dev/urandom", "r");
	char rand[128];
	char out[129];
	out[128] = 0;

	if (!urand)
		return 1;

	for (;;) {
		if (fread (rand, 128, 1, urand) > 0) {
			if ( qname_check(rand, 128) > 0 ) {
				qname_to_fqdn ( rand, 128, out, 128);
				printf("Valid %s\n", out);
			}
		}
	}*/

	char in[128];
	char out[128];

	strncpy ( in, "sub.domain.example.com\0", 127);

	printf("%s\n", in);

	int written = fqdn_to_qname (in,128,out,128);

	if ( qname_check(out,128) < 0)
		printf("Wrong\n");
	else
		printf("qname ok\n");

	if (written < 0) {
		printf("invallid fqdn\n");
		return 1;
	}

	for(int i = 0; i < written; i++)
		printf(" %x ", out[i]);

	written = qname_to_fqdn (out,128,in,128);

	if (written < 0) {
		printf("invalid qname\n");
		return 1;
	}

	printf("%s\n", in);

	printf("\n\n");
	return 0;
}

int main(	int argc,
		char* argv[] )
{
	int		ret;
	struct		sockaddr_in sock_server_addr;

	char recv_buffer[ UDP_BUFFER_LEN ];

	signal ( SIGTERM, signal_term );
	signal ( SIGINT,  signal_term );

	//Avoid zombie processes
	signal (SIGCHLD, SIG_IGN);

	log_init_stdout ( _LOG_DEBUG );

	sock_server = socket ( AF_INET, SOCK_DGRAM, 0 );
	if ( sock_server == -1 ) {
		LOGPRINTF(_LOG_ERROR, "socket() failed");
		return errno;
	}

	memset( &sock_server_addr, '\0', sizeof(struct sockaddr_in) );
	sock_server_addr.sin_family = AF_INET;
	sock_server_addr.sin_port   = htons( 53 );
	ret = inet_aton ( "0.0.0.0", & sock_server_addr.sin_addr );
	if( ret == 0 ) { //Error on 0, no errno!
		LOGPRINTF(_LOG_NOTE, "inet_aton(): Invalid bind IP\n" );
		return 1;
	}

	ret = bind (	sock_server,
			(struct sockaddr*) &sock_server_addr,
			sizeof(struct sockaddr_in) );
	if ( ret == -1 ) {
		LOGPRINTF(_LOG_ERROR, "bind() failed");
		return errno;
	}

	while( 1 )
	{
		struct 		sockaddr_in sock_client_addr;
		socklen_t	sock_client_addr_len;

		sock_client_addr_len = sizeof ( struct sockaddr_in );
		memset ( &sock_client_addr, '\0', sock_client_addr_len );

		ret = recvfrom (sock_server,
				recv_buffer,
				UDP_BUFFER_LEN,
				0,
				(struct sockaddr*) &sock_client_addr,
				&sock_client_addr_len );
		if ( ret == -1 ) {
			LOGPRINTF( _LOG_ERROR, "recvfrom()");
			return errno;
		}

		LOGPRINTF(_LOG_DEBUG, "UDP Packet size %i", ret);

		handle_connection ( sock_server,
				&sock_client_addr,
				sock_client_addr_len,
				recv_buffer,
				ret );
	}

	close( sock_server );

	return 0;
}

int handle_connection (	int _socket,
			struct sockaddr_in *sockaddr_client,
			socklen_t sockaddr_client_len,
			char* _buffer,
			int _bufflen )
{
	struct dns_message msg;

	if (dns_parse_packet (_buffer, _bufflen, &msg) ) {
		LOGPRINTF (_LOG_DEBUG, "Malformed packet recieved. parsing failed");
		return 1;
	}

	if(msg.question_count > 0) {
		char out[128];
		qname_to_fqdn( (char*) msg.question[0].qname, 100, out, 128);
		printf("%s %i\n", out, msg.question[0].qtype);
	}
	
	dns_destroy_struct ( &msg );

	//Always return NXDOMAIN
	struct dns_header head = {msg.header.id,1,OP_Q,0,0,0,0,0,NAMEERR,0,0,0,0};
	char ret[20];
	int retlen = dns_construct_header ( &head, ret, 20 );
	sendto (_socket, ret, retlen, 0, (struct sockaddr*) sockaddr_client, sockaddr_client_len);
	
	return 0;
}

void signal_term ( ) {
	printf( "Recieved Signal. Terminating active connections and closing socket\n" );

	//terminate all children >:)
	kill ( 0, SIGTERM ); 

	shutdown ( sock_server, SHUT_RDWR );
	close ( sock_server );

	printf( "done\n" );

	exit( 0 );
}

void signal_term_child ( ) {
	close ( sock_server );
	printf ( "%i: Active connection terminated\n", getpid() );
	exit( 0 );
}
