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

#define PRINT_ERRNO() {printf("%s:%i %i:%s\n", __FILE__, __LINE__, errno, strerror(errno));}

#define UDP_BUFFER_LEN 512

struct dns_message {
	//Header
	uint16_t id;

	uint8_t QR;	//Query:0 Reply:1
	uint8_t OPCODE;	//Query:0 Iquery:1 Status:2
	uint8_t AA;	//Authorative answer
	uint8_t TC;	//Truncation
	uint8_t RD;	//Recursion Desired
	uint8_t RA;	//Recursion Available
	uint8_t RCODE;	//Response Code

	uint16_t question_count;
	uint16_t answer_count;
	uint16_t authorative_count;
	uint16_t additional_count;

	//Question
	//Answer
 };

int sock_server;

int dns_parse_packet ( 	int _socket,
			struct sockaddr_in *sockaddr_client,
			socklen_t sockaddr_client_len,
			char* buffer,
			int bufflen );

void signal_term ( );

void signal_term_child ( );

int test_main(	int argc,
		char* argv[] )
{
	printf("TEST MODE. NOT FUNCTIONAL\n");

	char in[128];
	char out[128];

	strncpy ( in, "aaa.aaaaa.aa\0", 127);

	printf("%s\n", in);

	int written = fqdn_to_qname (in,128,out,128);

	if (written < 0) {
		printf("invallid fqdn\n");
		return 1;
	}

	for(int i = 0; i < written; i++)
		printf(" %x ", out[i]);

	printf("\n\n");
	return 0;
}

int main(	int argc,
		char* argv[] )
{
	int 		ret;
       	struct		sockaddr_in sock_server_addr;
	
	char recv_buffer[ UDP_BUFFER_LEN ];

	signal ( SIGTERM, signal_term );
	signal ( SIGINT,  signal_term );

	log_init_stdout ( _LOG_DEBUG );

	sock_server = socket ( AF_INET, SOCK_DGRAM, 0 );
	if ( sock_server == -1 ) {
		PRINT_ERRNO();
		return errno;
	}

	memset( &sock_server_addr, '\0', sizeof(struct sockaddr_in) );
	sock_server_addr.sin_family = AF_INET;
	sock_server_addr.sin_port   = htons( 53 );
	ret = inet_aton ( "0.0.0.0", & sock_server_addr.sin_addr );
	if( ret == 0 ) { //Error on 0, no errno!
		LOGPRINTF(_LOG_NOTE, "inet_aton(): Invalid IP\n" );
		return 1;
	}

	ret = bind (	sock_server,
			(struct sockaddr*) &sock_server_addr,
			sizeof(struct sockaddr_in) );
	if ( ret == -1 ) {
		PRINT_ERRNO();
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

		LOGPRINTF(_LOG_NOTE, "Connection");

		if ( dns_parse_packet ( sock_server,
					&sock_client_addr,
					sock_client_addr_len,
					recv_buffer,
					ret ) ) {
			LOGPRINTF(_LOG_ERROR, "dns_parse_packet()");
			return errno;
		}
		else {
			LOGPRINTF ( _LOG_DEBUG, "forked\n" );
		}
	}

	close( sock_server );

	return 0;
}

int dns_parse_packet (	int _socket,
			struct sockaddr_in *sockaddr_client,
			socklen_t sockaddr_client_len,
			char* buffer,
			int bufflen )
{
	pid_t pid = fork();
	
	if ( pid > 0)
		return 0;
	else if ( pid < 0 )
		return errno;

	signal ( SIGTERM, signal_term_child);

	struct dns_message msg;

	msg.id = *( (uint16_t*) buffer );
	msg.QR =  0x80 & *( (uint8_t*) (buffer + 2));

	exit ( 0 );
}

void signal_term ( ) {
	printf( "Recieved Signal. Terminating active connections and closing socket\n" );
	
	kill ( 0, SIGTERM ); //terminate all child processes

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
