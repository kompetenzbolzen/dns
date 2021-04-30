/* main.c
 * (c) Jonas Gunz, 2020
 * License: MIT
 * */

#include "server.h"

void run_dns_server ( server_config_t* _config )
{
	int		ret;
	struct		sockaddr_in sock_server_addr;

	char recv_buffer[ UDP_BUFFER_LEN ];

	struct database zone_db;

	signal ( SIGTERM, signal_term );
	signal ( SIGINT,  signal_term );

	//Avoid zombie processes
	//TODO currently useless, since no forking is done.
	signal (SIGCHLD, SIG_IGN);

	log_init_stdout ( _LOG_DEBUG );

	if ( (ret = database_populate ( &zone_db, "/nofile" )) ) {
		LOGPRINTF(_LOG_ERROR, "Failed to populate database from zonefile");
		exit(1);
	}

	LOGPRINTF(_LOG_NOTE, "Initializing DNS Server on %s:%i", _config->bind_ip, _config->bind_port);

	sock_server = socket ( AF_INET, SOCK_DGRAM, 0 );
	if ( sock_server == -1 ) {
		LOGPRINTF(_LOG_ERROR, "socket() failed");
		exit ( errno );
	}

	memset( &sock_server_addr, '\0', sizeof(struct sockaddr_in) );
	sock_server_addr.sin_family = AF_INET;
	sock_server_addr.sin_port   = htons( _config->bind_port );
	ret = inet_aton ( _config->bind_ip, & sock_server_addr.sin_addr );
	if( ret == 0 ) { //Error on 0, no errno!
		LOGPRINTF(_LOG_NOTE, "inet_aton(): Invalid bind IP\n" );
		exit ( 1 );
	}

	ret = bind (	sock_server,
			(struct sockaddr*) &sock_server_addr,
			sizeof(struct sockaddr_in) );
	if ( ret == -1 ) {
		LOGPRINTF(_LOG_ERROR, "bind() failed");
		exit ( errno );
	}

	LOGPRINTF(_LOG_NOTE, "Done!");

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
			exit ( errno );
		}

		LOGPRINTF(_LOG_DEBUG, "UDP Packet size %i", ret);

		handle_connection ( sock_server,
				&sock_client_addr,
				sock_client_addr_len,
				recv_buffer,
				ret,
				&zone_db );
	}

	close( sock_server );
}

int handle_connection (	int _socket,
			struct sockaddr_in *sockaddr_client,
			socklen_t sockaddr_client_len,
			char* _buffer,
			int _bufflen,
			struct database* _zone_db )
{
	struct dns_message msg;

	if (dns_parse_packet (_buffer, _bufflen, &msg) ) {
		LOGPRINTF (_LOG_DEBUG, "Malformed packet recieved. parsing failed");
		return 1;
	}

	if(msg.question_count > 0) {
		char out[128];
		qname_to_fqdn( (char*) msg.question[0].qname, 100, out, 128);
		LOGPRINTF(_LOG_DEBUG, "Request for %s QTYPE %i", out, msg.question[0].qtype);
	}

	// Only handles first request
	// TODO heavy refactoring. major POC vibe

	struct database_rdata rdata;
	struct dns_question* quest = & msg.question[0];

	int db_ret = database_query( &rdata, _zone_db, quest->qname, quest->qname_len, quest->qtype, quest->qclass );
	if (db_ret) {

		LOGPRINTF(_LOG_DEBUG, "DB Query exited with code %i", db_ret);
		return 1;
	}

	struct dns_header head = {msg.header.id,1,OP_QUERY,0,0,0,0,0,RCODE_NOERR,0,1,0,0};
	struct dns_answer answ = {quest->qname, quest->qname_len, RR_A, CL_IN, rdata.ttl, rdata.rdlen, rdata.rdata };

	char ret[512];
	int hlen = dns_construct_header ( ret, 512, &head );
	int alen = dns_construct_answer ( ret + hlen, 512-hlen, &answ );
	sendto (_socket, ret, hlen + alen, 0, (struct sockaddr*) sockaddr_client, sockaddr_client_len);

	dns_destroy_struct ( &msg );

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
