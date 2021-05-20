/* main.c
 * (c) Jonas Gunz, 2020
 * License: MIT
 * */

#include "server.h"

void server_start ( server_config_t* _config )
{
	fd_set sel_fds;
	struct timeval sel_interval;
	int    sel_ret = 0;

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
		FD_ZERO ( &sel_fds );
		FD_SET  ( sock_server, &sel_fds );
		sel_interval.tv_sec  = 0;
		sel_interval.tv_usec = 10000;

		sel_ret = select( sock_server + 1, &sel_fds, NULL, NULL, &sel_interval );

		if ( sel_ret < 0 ) {
			LOGPRINTF( _LOG_ERRNO, "select()" );
			exit(1);
		} else if ( sel_ret ) {
			// A connection is available
			DEBUG("Connection");
			server_handle_connection( sock_server, &zone_db );
		}
	}

	close( sock_server );
	exit(0);
}

void server_handle_connection ( int _socket, database_t* _zone_db ) {
	char recv_buffer[ UDP_BUFFER_LEN ];
	int  recv_len = 0;

	char answ_buffer[ UDP_BUFFER_LEN ];
	int  answ_len = UDP_BUFFER_LEN;
	int  answ_cnt = DNS_HEADER_LEN;
	// preload with header length, because it is written last.

	struct    sockaddr_in sock_client_addr;
	socklen_t sock_client_addr_len = sizeof( struct sockaddr_in );

	dns_message_t dns_req;
	dns_header_t answ_header;

	memset ( &sock_client_addr, 0, sock_client_addr_len );

	recv_len = recvfrom ( _socket, recv_buffer, UDP_BUFFER_LEN,
			0, (struct sockaddr*) &sock_client_addr,
			&sock_client_addr_len );

	if ( recv_len == -1 ) {
		LOGPRINTF( _LOG_ERRNO, "recvfrom()");
		exit ( 1 );
	}

	if ( dns_parse_packet( recv_buffer, recv_len, &dns_req ) ) {
		DEBUG("Malformed packet recieved. parsing failed");
		// free?
		return;
	}

	if ( ! dns_req.question_count ) {
		DEBUG("No questions in request.");
		goto end;
	}

	DEBUG("Valid data with %i question(s)", dns_req.question_count);

	memset ( &answ_header, 0, sizeof( dns_header_t ) );

	answ_header.id = dns_req.header.id;
	answ_header.QR = 1; //Response
	answ_header.AA = 1;
	
	// TODO test with artificially large rdata to exceed buffer
	for (unsigned int i = 0; i < dns_req.question_count; i++) {
		int cnt_inc = 0;
		database_rdata_t db_rdata;
		dns_question_t *quest = &dns_req.question[i];
		dns_answer_t dns_answ = {quest->qname, quest->qname_len, quest->qtype, quest->qclass, 0, 0, NULL };


		if( database_query( &db_rdata, _zone_db, quest->qname, quest->qname_len, quest->qtype, quest->qclass ) ) {
			answ_header.RCODE = RCODE_NAMEERR;
			DEBUG("Could not answer question %i", i);
			continue;
		}

		dns_answ.rdlength = db_rdata.rdlen;
		dns_answ.rdata    = db_rdata.rdata;
		dns_answ.ttl      = db_rdata.ttl;

		cnt_inc += dns_construct_answer( &answ_buffer[answ_cnt], answ_len - answ_cnt, &dns_answ );
		
		if (cnt_inc <= 0) {
			LOGPRINTF(_LOG_ERROR, "dns_construct_answer() return <= 0");
			goto end;
		}

		answ_cnt += cnt_inc;
		answ_header.answer_count += 1;
	}

	dns_construct_header( answ_buffer, answ_len, &answ_header );

	sendto( _socket, answ_buffer, answ_cnt, 0, (struct sockaddr*) &sock_client_addr, sock_client_addr_len );

end:
	dns_destroy_struct ( &dns_req );
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
