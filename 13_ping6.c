/* libnet 1.1 tutorial (https://repolinux.wordpress.com/category/libnet/) sample code
 * Example 13: ping6.c */

#include <stdio.h>
#include <stdlib.h>
#include <libnet.h>
#include <stdint.h>

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

	libnet_t *l;	/* libnet context */
	char errbuf[LIBNET_ERRBUF_SIZE];

	char ip_addr_str[MAX_CHARS_IN_IPV6_ADDR+1];
	struct libnet_in6_addr ip_dst_addr;
	char payload[] = "libnet :D";
	u_int16_t id, seq;
	int bytes_written;

	l = libnet_init(LIBNET_RAW6, "eth0", errbuf);
	if ( l == NULL ) {
		fprintf(stderr, "libnet_init() failed: %s\n", errbuf);
		exit(EXIT_FAILURE);
	}

	/* Generating a random id */

	libnet_seed_prand (l);
	id = (u_int16_t)libnet_get_prand(LIBNET_PR16);

	/* Getting destination IP address */

	ip_addr_str[0] = '\0';

	printf("Destination IPv6 address: ");
	scanf("%39s",ip_addr_str); /* too lazy not to hardcode this */

	ip_dst_addr = libnet_name2addr6(l, ip_addr_str, LIBNET_DONT_RESOLVE);

	if (libnet_in6_addr_cmp(ip_dst_addr, in6addr_error)) {
		fprintf(stderr, "Error converting IPv6 address.\n");
		libnet_destroy(l);
		exit(EXIT_FAILURE);
	}

	/* Building ICMP header */

	seq = 1;

	if (libnet_build_icmpv6_echo(ICMP6_ECHO, 0, 0, id, seq,\
				(u_int8_t*)payload, sizeof(payload), l, 0) == -1)
	{
		fprintf(stderr, "Error building ICMPv6 header: %s\n",\
				libnet_geterror(l));
		libnet_destroy(l);
		exit(EXIT_FAILURE);
	}
	
	/* Building IP header */
	
	if ( libnet_autobuild_ipv6(LIBNET_ICMPV6_ECHO_H + sizeof(payload), IPPROTO_ICMP6, \
				ip_dst_addr, l, 0) == -1 )
	{
		fprintf(stderr, "Error building IPv6 header: %s\n",\
				libnet_geterror(l));
		libnet_destroy(l);
		exit(EXIT_FAILURE);
	}

	/* Writing packet */

	bytes_written = libnet_write(l);
	if ( bytes_written != -1 )
		printf("%d bytes written.\n", bytes_written);
	else
		fprintf(stderr, "Error writing packet: %s\n",\
				libnet_geterror(l));

	libnet_destroy(l);
	return 0;
}
/* EOF */
