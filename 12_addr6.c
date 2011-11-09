/* libnet 1.1 tutorial (https://repolinux.wordpress.com/category/libnet/) sample code
 * Example 12: addr6.c */

#include <stdio.h>
#include <stdlib.h>
#include <libnet.h>
#include <stdint.h>

#define BYTES_IN_IPV6_ADDR 16
#define MAX_CHARS_IN_IPV6_ADDR 39

int libnet_in6_addr_cmp(struct libnet_in6_addr addr1, \
		struct libnet_in6_addr addr2) {
	/* Returns != 0 if addresses are equal, 0 otherwise. */

	uint32_t *p1 = (uint32_t*)&addr1.__u6_addr, \
		 *p2 = (uint32_t*)&addr2.__u6_addr;

	return ((p1[0] == p2[0]) && (p1[1] == p2[1]) && \
			(p1[2] == p2[2]) && (p1[3] == p2[3]));
}

int main() {

	libnet_t *l; /* the libnet context */
	char errbuf[LIBNET_ERRBUF_SIZE];
	int i;

	char ipv6_addr_str[MAX_CHARS_IN_IPV6_ADDR+1];
	struct libnet_in6_addr ipv6_addr;
	u_int8_t *ipv6_addr_p;

	l = libnet_init(LIBNET_RAW6, NULL, errbuf);
	if ( l == NULL ) {
	fprintf(stderr, "libnet_init() failed: %s\n", errbuf);
		exit(EXIT_FAILURE);
	}

	printf("Enter an IPv6 address: ");
	scanf("%39s",ipv6_addr_str); /* too lazy not to hardcode this */

	ipv6_addr = libnet_name2addr6(l, ipv6_addr_str, LIBNET_DONT_RESOLVE);

	if (libnet_in6_addr_cmp(ipv6_addr, in6addr_error) != 0) {
		fprintf(stderr, "Error converting IPv6 address.\n");
		libnet_destroy(l);
		exit(EXIT_FAILURE);
	}

	ipv6_addr_p = (u_int8_t*)&ipv6_addr.__u6_addr;
	printf("libnet_name2addr6() returned: ");
	for ( i=0; i < BYTES_IN_IPV6_ADDR; i++) {
		printf("%02x", ipv6_addr_p[i]);
		if ((i % 2 == 1) && i < BYTES_IN_IPV6_ADDR -1)
			printf(":");
	}
	printf("\n");

	ipv6_addr_str[0] = '\0';
	libnet_addr2name6_r(ipv6_addr, LIBNET_DONT_RESOLVE, ipv6_addr_str, \
			MAX_CHARS_IN_IPV6_ADDR);
	printf("libnet_addr2name6() says it's: %s\n", ipv6_addr_str);

	libnet_destroy(l);
	return 0;
}
/* EOF */
