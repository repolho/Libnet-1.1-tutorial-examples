/* libnet 1.1 tutorial (https://repolinux.wordpress.com/category/libnet/) sample code
 * Example 4: get_own_addr.c */

#include <stdio.h>
#include <stdlib.h>
#include <libnet.h>
#include <stdint.h>

int main() {

	libnet_t *l;	/* libnet context */
	char errbuf[LIBNET_ERRBUF_SIZE];
	u_int32_t ip_addr;
	struct libnet_ether_addr *mac_addr;

	l = libnet_init(LIBNET_RAW4, NULL, errbuf);
	if ( l == NULL ) {
		fprintf(stderr, "libnet_init() failed: %s\n", errbuf);
		exit(EXIT_FAILURE);
	}

	ip_addr = libnet_get_ipaddr4(l);
	if ( ip_addr != -1 )
		printf("IP address: %s\n", libnet_addr2name4(ip_addr, LIBNET_DONT_RESOLVE));
	else
		fprintf(stderr, "Couldn't get own IP address: %s\n", libnet_geterror(l));

	mac_addr = libnet_get_hwaddr(l);
	if ( mac_addr != NULL )
		printf("MAC address: %02X:%02X:%02X:%02X:%02X:%02X\n",\
				mac_addr->ether_addr_octet[0],\
				mac_addr->ether_addr_octet[1],\
				mac_addr->ether_addr_octet[2],\
				mac_addr->ether_addr_octet[3],\
				mac_addr->ether_addr_octet[4],\
				mac_addr->ether_addr_octet[5]);
	else
		fprintf(stderr, "Couldn't get own MAC address: %s\n", libnet_geterror(l));

	libnet_destroy(l);
	return 0;
}
/* EOF */
