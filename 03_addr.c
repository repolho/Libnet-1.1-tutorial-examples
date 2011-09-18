/* libnet 1.1 tutorial (https://repolinux.wordpress.com/category/libnet/) sample code
 * Example 3: addr.c */

#include <stdio.h>
#include <stdlib.h>
#include <libnet.h>
#include <stdint.h>

int main() {

	libnet_t *l;	/* the libnet context */
	char errbuf[LIBNET_ERRBUF_SIZE];
	char ip_addr_str[16], mac_addr_str[18];
	u_int32_t ip_addr;
	u_int8_t *ip_addr_p, *mac_addr;
	int i, length;	/* for libnet_hex_aton() */

	l = libnet_init(LIBNET_RAW4, NULL, errbuf);
	if ( l == NULL ) {
		fprintf(stderr, "libnet_init() failed: %s\n", errbuf);
		exit(EXIT_FAILURE);
	}

	/* IP address */

	printf("IP address: ");
	scanf("%15s",ip_addr_str);

	ip_addr = libnet_name2addr4(l, ip_addr_str, LIBNET_DONT_RESOLVE);

	if ( ip_addr != -1 ) {

		/* ip_addr is ready to be used in a build function.
		 * We'll print its contents to stdout to check if everything
		 * went fine. */

		/* libnet_name2addr4 returns the address in network order (big
		 * endian). */
		
		ip_addr_p = (u_int8_t*)(&ip_addr);
		/* Check your system's endianess: */
		/*
		printf("ip_addr:   %08X\n", ip_addr);
		printf("ip_addr_p: %02X%02X%02X%02X\n", ip_addr_p[0],\
				ip_addr_p[1], ip_addr_p[2], ip_addr_p[3]);
		*/

		printf("Address read: %d.%d.%d.%d\n", ip_addr_p[0],\
				ip_addr_p[1], ip_addr_p[2], ip_addr_p[3]);

		/* This would output the same thing, but I wanted to show you
		 * how the address is stored in memory. */
		/*
		printf("Address read: %s\n", libnet_addr2name4(ip_addr,\
				LIBNET_DONT_RESOLVE));
		*/
	}
	else
		fprintf(stderr, "Error converting IP address.\n");

	/* MAC address */

	printf("MAC address: ");
	scanf("%17s", mac_addr_str);

	mac_addr = libnet_hex_aton(mac_addr_str, &length);

	if (mac_addr != NULL) {

		/* mac_addr is ready to be used in a build function.
		 * We'll print its contents to stdout to check if everything
		 * went fine. */

		printf("Address read: ");
		for ( i=0; i < length; i++) {
			printf("%02X", mac_addr[i]);
			if ( i < length-1 )
				printf(":");
		}
		printf("\n");

		/* Remember to free the memory allocated by libnet_hex_aton() */
		free(mac_addr);
	}
	else
		fprintf(stderr, "Error converting MAC address.\n");


	libnet_destroy(l);
	return 0;
}
/* EOF */
