/* main.c
 * (c) Jonas Gunz, 2020
 * License: MIT
 * */

#include "server.h"

void server_start ( server_config_t* _config )
{
	char recv_buffer[ UDP_BUFFER_LEN ];
	database_t zone_db;

	signal ( SIGTERM, signal_term );
	signal ( SIGINT,  signal_term );

	if ( database_populate( &zone_db, _config->zonefile ) ) {
		LOGPRINTF(_LOG_ERRNO, "Failed to populate database from zonefile");
		exit(1);
	}

	sock_server = server_get_socket( _config->bind_ip, _config->bind_port );

	LOGPRINTF(_LOG_NOTE, "Done!");

	while( 1 ) {
		struct    sockaddr_in sock_client_addr;
		socklen_t sock_client_addr_len = sizeof( struct sockaddr_in );
		int       recv_len = 0;

		memset ( &sock_client_addr, 0, sock_client_addr_len );

		recv_len = recvfrom (sock_server,
				recv_buffer,
				UDP_BUFFER_LEN,
				0,
				(struct sockaddr*) &sock_client_addr,
				&sock_client_addr_len );
		if ( recv_len == -1 ) {
			LOGPRINTF( _LOG_ERRNO, "recvfrom()");
			exit ( errno );
		}

		DEBUG("Packet size %i from %s:%i", recv_len, inet_ntoa(sock_client_addr.sin_addr), sock_client_addr.sin_port );

		handle_connection ( sock_server,
				&sock_client_addr,
				sock_client_addr_len,
				recv_buffer,
				recv_len,
				&zone_db );
	}

	close( sock_server );
	exit(0);
}

int handle_connection (	int _socket,
			struct sockaddr_in *sockaddr_client,
			socklen_t sockaddr_client_len,
			char* _buffer,
			int _bufflen,
			database_t* _zone_db ) {
	dns_message_t msg;

	if ( dns_parse_packet (_buffer, _bufflen, &msg) ) {
		DEBUG("Malformed packet recieved. parsing failed");
		return 1;
	}

	if ( ! msg.question_count ) {
		DEBUG("No questions in request.");
		return 1;
	}

	if (msg.question_count > 0) {
		char out[128];
		qname_to_fqdn( (char*) msg.question[0].qname, msg.question[0].qname_len, out, 128);
		LOGPRINTF(_LOG_DEBUG, "Request for %s QTYPE %i", out, msg.question[0].qtype);
	}

	// Only handles first request
	// TODO heavy refactoring. major POC vibe

	database_rdata_t rdata;
	dns_question_t* quest = & msg.question[0];

	int db_ret = database_query( &rdata, _zone_db, quest->qname, quest->qname_len, quest->qtype, quest->qclass );
	if (db_ret) {
		LOGPRINTF(_LOG_DEBUG, "DB Query exited with code %i", db_ret);
		dns_destroy_struct ( &msg );
		return 1;
	}

	dns_header_t head = {msg.header.id,1,OP_QUERY,0,0,0,0,0,RCODE_NOERR,0,1,0,0};
	dns_answer_t answ = {quest->qname, quest->qname_len, RR_A, CL_IN, rdata.ttl, rdata.rdlen, rdata.rdata };

	char ret[512];
	int hlen = dns_construct_header ( ret, 512, &head );
	int alen = dns_construct_answer ( ret + hlen, 512-hlen, &answ );
	sendto( _socket, ret, hlen + alen, 0, (struct sockaddr*) sockaddr_client, sockaddr_client_len );

	dns_destroy_struct ( &msg );

	return 0;
}

int server_get_socket ( char* _bind_ip, uint16_t _bind_port ) {
	struct sockaddr_in socket_addr;
	int server_socket;

	LOGPRINTF(_LOG_NOTE, "Binding on %s:%i", _bind_ip, _bind_port);

	server_socket = socket ( AF_INET, SOCK_DGRAM, 0 );
	if ( server_socket == -1 ) {
		LOGPRINTF(_LOG_ERRNO, "failed to create socket");
		exit ( errno );
	}

	memset( &socket_addr, '\0', sizeof(struct sockaddr_in) );

	socket_addr.sin_family = AF_INET;
	socket_addr.sin_port   = htons( _bind_port );

	if ( ! inet_aton ( _bind_ip, & socket_addr.sin_addr ) ) {
		LOGPRINTF(_LOG_ERROR, "inet_aton(): Invalid bind IP\n" );
		exit ( 1 );
	}

	if ( bind( server_socket, (struct sockaddr*) &socket_addr, sizeof(struct sockaddr_in) ) ) {
		LOGPRINTF(_LOG_ERRNO, "bind() failed");
		exit ( errno );
	}

	return server_socket;
}

void signal_term ( ) {
	LOGPRINTF(_LOG_NOTE, "Server shutting down" );

	shutdown ( sock_server, SHUT_RDWR );
	close ( sock_server );

	LOGPRINTF(_LOG_NOTE, "Done!" );
	exit( 0 );
}
