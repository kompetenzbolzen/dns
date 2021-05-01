/* test.c
 * (c) Jonas Gunz, 2020
 * License: MIT
 * */

#include "test.h"

#ifdef _TEST

void run_test ()
{
	log_init_stdout(_LOG_DEBUG);
	//Space for temporary tests

	//tree_balanced_insert(NULL, NULL, NULL, 15 );
	//
	//Normal tests
	
	int ret = 0;

	ret += test_tree();
	ret += test_dns_parsing();
	ret += test_dns_message_fuzz();
	ret += test_dns_qname_fuzz();
	ret += test_database();

	exit(ret);
}

int test_tree ()
{
	unsigned const int len = pow ( 'z' - 'a' + 1, 2);
	unsigned int len_cnt = 0;
	char* keys[len];
	char* data[len];

	struct tree_node* root = NULL;

	printf("\n-> test_tree()\n======\n\n");

	for ( char i = 'a'; i <= 'z'; i++ ) {
		for ( char j = 'a'; j <= 'z'; j++ ) {
			keys[len_cnt] = malloc (3);
			keys[len_cnt][0] = i;
			keys[len_cnt][1] = j;
			keys[len_cnt][2] = 0;

			data[len_cnt] = malloc(10);
			snprintf( data[len_cnt], 10, "N%i", len_cnt );

			len_cnt ++;
		}
	}

	printf("len_cnt %i\n", len_cnt);

	tree_balanced_insert( &root, (void**)data, keys, len );

	printf("After Insert\n");

	printf("%s\n", (char*)tree_get(&root, "aa"));

	for ( int i = 0; i < len; i++ ) {
		if ( strcmp( tree_get(&root, keys[i]), data[i] ) )
			LOGPRINTF(_LOG_WARNING, "Data does not match for %s", keys[i]);
	}

	printf("After Get\n");

	tree_destroy (&root, _TREE_FREE_DATA | _TREE_FREE_KEY);

	printf("After destroy\n");

	return 0;
}

int test_dns_parsing ()
{
	printf("\n-> test_dns_parsing()\n======\n\n");
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

int test_dns_qname_fuzz()
{
	printf("\n-> test_parsing_fuzz()\n======\n\n");
	FILE* urand = fopen ("/dev/urandom", "r");
	char rand[128];
	char out[129];
	out[128] = 0;

	if (!urand)
		return 1;

	unsigned long int limit = 10000000;
	unsigned long int valid_cnt = 0;
	for (unsigned long int i = 0; i < limit; i++) {
		if (fread (rand, 128, 1, urand) > 0) {
			if ( qname_check(rand, 128) > 0 ) {
				qname_to_fqdn ( rand, 128, out, 128);
				//printf("Valid %s\n", out);
				valid_cnt++;
			}
		}
	}

	float valid_percent = (float)valid_cnt / (float)limit * 100;
	printf("# of valid qnames in random data: %lu / %lu = %f%%\n", valid_cnt, limit, valid_percent);

	return 0;
}

int test_dns_message_fuzz()
{
	printf("\n-> test_dns_message_fuzz()\n======\n\n");
	FILE* urand = fopen ("/dev/urandom", "r");
	char rand[128];

	if (!urand)
		return 1;

	unsigned long int limit = 10000000;
	unsigned long int valid_cnt = 0;
	struct dns_message msg;

	for (unsigned long int i = 0; i < limit; i++) {
		if (fread (rand, 128, 1, urand) > 0) {
			if ( ! dns_parse_packet(rand, 128, &msg) ) {
				valid_cnt++;
			}
		}
	}

	float valid_percent = (float)valid_cnt / (float)limit * 100;
	printf("# of valid messages in random data: %lu / %lu = %f%%\n", valid_cnt, limit, valid_percent);

	return 0;
}

int test_database(){
	struct database db;
	struct database_rdata rdata;

	if ( database_populate( &db, "nofile" ) )
		return 1;

	printf("Populated\n");

	char* qname = malloc(32);
	int len = fqdn_to_qname( "test.example.com.", 18, qname, 32 );

	int ret = database_query ( &rdata, &db, qname, len, 1, 1 );
	printf("Return code %i, rdlen %i\n", ret, rdata.rdlen);

	database_destroy( &db );

	return 0;
}

#endif
