/* test.c
 * (c) Jonas Gunz, 2020
 * License: MIT
 * */

#include "test.h"

#ifdef _TEST

void run_test ()
{
	//Space for temporary tests
	
	//tree_balanced_insert(NULL, NULL, NULL, 15 );
	//Normal tests
	test_tree();
	//test_dns_parsing();
	test_dns_message_fuzz();
	test_dns_qname_fuzz();
}

int test_tree ()
{
	printf("\n-> test_tree()\n======\n\n");

	struct tree_node* root = NULL;
	
	tree_insert ( &root, "eins", "Test eins" );
	tree_insert ( &root, "zwei", "Test zwei" );
	
	printf("After Insert\n");

	printf("%s\n", tree_get(&root, "zwei"));

	printf("After Get\n");

	tree_destroy (&root,0);

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
	printf("# of valid qnames in random data: %i / %i = %f%%\n", valid_cnt, limit, valid_percent);

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
	printf("# of valid messages in random data: %i / %i = %f%%\n", valid_cnt, limit, valid_percent);

	return 0;
}
#endif
