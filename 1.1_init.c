/* libnet 1.1 tutorial (http://repura.livejournal.com/31673.html) sample code
 * Part 1 - example 1: init.c */

#include <stdio.h>
#include <stdlib.h>
#include <libnet.h>

int main() {

	libnet_t *l;	// the libnet context
	char errbuf[LIBNET_ERRBUF_SIZE];

	l = libnet_init(LIBNET_RAW4, NULL, errbuf);
        if ( l == NULL ) {
		fprintf(stderr, "libnet_init() failed: %s\n", errbuf);
		exit(EXIT_FAILURE);
	}

	libnet_destroy(l);
	return 0;
}
/* EOF */
