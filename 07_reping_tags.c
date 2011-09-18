/* libnet 1.1 tutorial (https://repolinux.wordpress.com/category/libnet/) sample code
 * Example 7: reping_tags.c */

#include <stdio.h>
#include <stdlib.h>
#include <libnet.h>
#include <stdint.h>
#include <unistd.h>

int main() {

	libnet_t *l;	/* libnet context */
	char errbuf[LIBNET_ERRBUF_SIZE], ip_addr_str[16];
	u_int32_t ip_addr;
	libnet_ptag_t icmp_tag, ip_tag;
	u_int16_t id, seq;
	int i;

	l = libnet_init(LIBNET_RAW4, NULL, errbuf);
	if ( l == NULL ) {
		fprintf(stderr, "libnet_init() failed: %s\n", errbuf);
		exit(EXIT_FAILURE);
	}

	icmp_tag = ip_tag = LIBNET_PTAG_INITIALIZER;

	/* Generating a random id */

	libnet_seed_prand(l);
	id = (u_int16_t)libnet_get_prand(LIBNET_PR16);

	/* Getting destination IP address */

	printf("Destination IP address: ");
	scanf("%15s",ip_addr_str);

	ip_addr = libnet_name2addr4(l, ip_addr_str, LIBNET_DONT_RESOLVE);

	if ( ip_addr == -1 ) {
		fprintf(stderr, "Error converting IP address.\n");
		libnet_destroy(l);
		exit(EXIT_FAILURE);
	}

	/* Building ICMP header */

	seq = 1;

	icmp_tag = libnet_build_icmpv4_echo(ICMP_ECHO, 0, 0, id, seq, NULL,\
			0, l, 0);

	if (icmp_tag == -1) {
		fprintf(stderr, "Error building ICMP header: %s\n",\
				libnet_geterror(l));
		libnet_destroy(l);
		exit(EXIT_FAILURE);
	}
	
	/* Building IP header */
	
	ip_tag = libnet_autobuild_ipv4(LIBNET_IPV4_H + LIBNET_ICMPV4_ECHO_H,\
			IPPROTO_ICMP, ip_addr, l);

	if (ip_tag == -1) {
		fprintf(stderr, "Error building IP header: %s\n",\
				libnet_geterror(l));
		libnet_destroy(l);
		exit(EXIT_FAILURE);
	}

	/* Writing 4 packets */

	for ( i = 0; i < 4; i++ ) {

		/* Updating the ICMP header */
		icmp_tag = libnet_build_icmpv4_echo(ICMP_ECHO, 0, 0, id,\
				(seq + i), NULL, 0, l, icmp_tag);

		if (icmp_tag == -1) {
			fprintf(stderr, "Error building ICMP header: %s\n",\
					libnet_geterror(l));
			libnet_destroy(l);
			exit(EXIT_FAILURE);
		}

		if ( libnet_write(l) == -1 )
			fprintf(stderr, "Error writing packet: %s\n",\
					libnet_geterror(l));

		/* Waiting 1 second between each packet */
		sleep(1);

	}

	libnet_destroy(l);
	return 0;

}
/* EOF */
