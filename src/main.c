#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#include <unistd.h>

#define PRINT_ERRNO() {printf("%s:%i %i:%s\n", __FILE__, __LINE__, errno, strerror(errno));}

#define UDP_BUFFER_LEN 512

int dns_parse_packet(int _socket, struct sockaddr_in *sockaddr_client, socklen_t sockaddr_client_len, char* buffer);

int main(int argc, char* argv[])
{
	int 		ret;
	int 		sock_server;
       	struct		sockaddr_in sock_server_addr;
	
	char recv_buffer[ UDP_BUFFER_LEN ];

	sock_server = socket ( AF_INET, SOCK_DGRAM, 0 );
	if ( sock_server == -1 ) {
		PRINT_ERRNO();
		return errno;
	}

	memset( &sock_server_addr, '\0', sizeof(struct sockaddr_in) );
	sock_server_addr.sin_family = AF_INET;
	sock_server_addr.sin_port   = htons( 53 );
	ret = inet_aton ( "0.0.0.0", & sock_server_addr.sin_addr );
	if( ret == 0 ) {//Error on 0, no errno
		printf("Invalid IP\n");
		return 1;
	}

	ret = bind( sock_server , &sock_server_addr, sizeof(struct sockaddr_in) );
	if ( ret == -1 ) {
		PRINT_ERRNO();
		return errno;
	}

	//while( 1 )
	{
		struct 		sockaddr_in sock_client_addr;
		socklen_t	sock_client_addr_len;

		sock_client_addr_len = sizeof ( struct sockaddr_in );
		memset ( &sock_client_addr, '\0', sock_client_addr_len );

		ret = recvfrom ( sock_server, recv_buffer, UDP_BUFFER_LEN, 0, &sock_client_addr, &sock_client_addr_len );
		if ( ret == -1 ) {
			PRINT_ERRNO();
			return errno;
		}
	}

	close( sock_server );

	return 0;
}


int dns_parse_packet(int _socket, struct sockaddr_in *sockaddr_client, socklen_t sockaddr_client_len, char* buffer) 
{
	pid_t = pid fork();
	
	if ( pid == 0)
		return 0;
	else if ( pid < 0 )
		return errno;

	exit (0);
}
