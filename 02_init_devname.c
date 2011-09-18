/* libnet 1.1 tutorial (https://repolinux.wordpress.com/category/libnet/) sample code
 * Example 2: init_devname.c */

#include <stdio.h>
#include <stdlib.h>
#include <libnet.h>

int main(int argc, char **argv) {

	libnet_t *l;	/* the libnet context */
	char errbuf[LIBNET_ERRBUF_SIZE];

	if ( argc == 1 ) {
		fprintf(stderr,"Usage: %s device\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	l = libnet_init(LIBNET_RAW4, argv[1], errbuf);
	if ( l == NULL ) {
		fprintf(stderr, "libnet_init() failed: %s\n", errbuf);
		exit(EXIT_FAILURE);
	}

	libnet_destroy(l);
	return 0;
}
/* EOF */
