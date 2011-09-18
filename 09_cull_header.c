/* libnet 1.1 tutorial (https://repolinux.wordpress.com/category/libnet/) sample code
 * Example 9: cull_header.c */

#include <stdio.h>
#include <stdlib.h>
#include <libnet.h>
#include <stdint.h>

int main() {

	/* Builds an IP header and prints it */

	libnet_t *l;
	char errbuf[LIBNET_ERRBUF_SIZE];
	libnet_ptag_t ip_tag;
	u_int8_t *ip_header;
	u_int32_t ip_header_size;
	int i;

	l = libnet_init(LIBNET_LINK_ADV, NULL, errbuf);
	if ( l == NULL ) {
		fprintf(stderr, "libnet_init() failed: %s\n", errbuf);
		exit(EXIT_FAILURE);
	}

	/* Building IP header, size = 20 bytes, dest address = 0.0.0.0, upper
	 * layer protocol = 0 */
	ip_tag = libnet_autobuild_ipv4(20, 0, 0, l);
	if ( ip_tag == -1 ) {
		fprintf(stderr, "Error building IP header: %s\n",\
				libnet_geterror(l));
		libnet_destroy(l);
		exit(EXIT_FAILURE);
	}
	
	/* Getting a pointer to the header */
	if (libnet_adv_cull_header(l, ip_tag, &ip_header, &ip_header_size)\
			== -1) {
		fprintf(stderr, "libnet_adv_cull_header() failed: %s\n",\
				libnet_geterror(l));
		libnet_destroy(l);
		exit(EXIT_FAILURE);
	}

	/* Printing the header */
	for (i=0; i < ip_header_size; i++) {
		printf("%02X ", ip_header[i]);
	}
	printf("\n");

	libnet_destroy(l);
	return 0;
}
/* EOF */
