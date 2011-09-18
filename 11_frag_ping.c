/* libnet 1.1 tutorial (https://repolinux.wordpress.com/category/libnet/) sample code
 * Example 11: frag_ping.c */

#include <stdio.h>
#include <stdlib.h>
#include <libnet.h>
#include <stdint.h>

#define MTU 1500

libnet_t *l;	/* libnet context */

void frag_and_send(u_int8_t *payload, u_int32_t total_pload_size);
u_int16_t get_sum(u_int8_t *payload, u_int32_t total_pload_size, \
		u_int16_t id, u_int16_t seq);

int main() {

	int i;
	char errbuf[LIBNET_ERRBUF_SIZE];
	/* It's a good idea to have the payload as an array of bytes. If yours
	 * isn't, make a pointer to it and cast it.*/
	u_int8_t payload[3000];

	l = libnet_init(LIBNET_RAW4, NULL, errbuf);
	if ( l == NULL ) {
		fprintf(stderr, "libnet_init() failed (raw4, 1st call): %s\n", \
				errbuf);
		exit(EXIT_FAILURE);
	}

	/* Generating random payload */
	libnet_seed_prand (l);

	for (i = 0; i < sizeof(payload); i++) {
		payload[i] = libnet_get_prand(LIBNET_PR8);
	}

	/* Building and sending the fragments */
	frag_and_send(payload, sizeof(payload));

	libnet_destroy(l);
	return 0;
}

void frag_and_send(u_int8_t *payload, u_int32_t total_pload_size) {

	/* Builds and sends the first packet, calling get_sum() to get the 
	 * correct checksum for the ICMP packet (with the whole payload). Then 
	 * builds and sends IP fragments until all the payload is sent. */

	char ip_addr_str[16];
	u_int32_t ip_addr, src_addr;
	u_int16_t id, seq, ip_id;
	/* hdr_offset = fragmentation flags + offset (in bytes) divided by 8 */
	int pload_offset, hdr_offset; 
	int bytes_written, max_pload_size, packet_pload_size;
	libnet_ptag_t ip_tag;

	/* Generating random IDs */

	id = (u_int16_t)libnet_get_prand(LIBNET_PR16);
	/* We need a non-zero id number for the IP headers, otherwise libnet
	 * will increase it after each build_ipv4, breaking the fragments */
	ip_id = (u_int16_t)libnet_get_prand(LIBNET_PR16);

	seq = 1;

	/* Getting IP addresses */

	src_addr = libnet_get_ipaddr4(l);
	if ( src_addr == -1 ) {
		fprintf(stderr, "Couldn't get own IP address: %s\n", \
				libnet_geterror(l));
		libnet_destroy(l);
		exit(EXIT_FAILURE);
	}

	printf("Destination IP address: ");
	scanf("%15s",ip_addr_str);

	ip_addr = libnet_name2addr4(l, ip_addr_str, LIBNET_DONT_RESOLVE);

	if ( ip_addr == -1 ) {
		fprintf(stderr, "Error converting IP address.\n");
		libnet_destroy(l);
		exit(EXIT_FAILURE);
	}

	/* Getting max payload size */

	max_pload_size = (MTU - LIBNET_IPV4_H);
	/* making it a multiple of 8 */
	max_pload_size -= (max_pload_size % 8);
	
	pload_offset = 0;

	/* Building the first packet, which carries the ICMP header */
	
	/* We're doing (payload size - icmp header size) and not
	 * checking if it's a multiple of 8 because we know the header is 8 
	 * bytes long */
	if ( total_pload_size > (max_pload_size - LIBNET_ICMPV4_ECHO_H) ) {
		hdr_offset = IP_MF;
		packet_pload_size = max_pload_size - LIBNET_ICMPV4_ECHO_H;
	}
	else {
		hdr_offset = 0;
		packet_pload_size = total_pload_size;
	}
	
	/* ICMP header */
	if ( libnet_build_icmpv4_echo(ICMP_ECHO, 0, get_sum(payload, \
					total_pload_size, id, seq), id, seq, \
				payload, packet_pload_size, l, 0) == -1 )
	{
		fprintf(stderr, "Error building ICMP header: %s\n", \
				libnet_geterror(l));
		libnet_destroy(l);
		exit(EXIT_FAILURE);
	}
	
	/* First IP header (no payload, offset == 0) */
	if ( libnet_build_ipv4((LIBNET_IPV4_H + LIBNET_ICMPV4_ECHO_H \
					+ packet_pload_size), 0, ip_id, \
				hdr_offset, 255, IPPROTO_ICMP, 0, src_addr, \
				ip_addr, NULL, 0, l, 0) == -1 )
	{
		fprintf(stderr, "Error building IP header: %s\n", \
				libnet_geterror(l));
		libnet_destroy(l);
		exit(EXIT_FAILURE);
	}

	/* Writing packet */

	bytes_written = libnet_write(l);

	if ( bytes_written != -1 )
		printf("%d bytes written.\n", bytes_written);
	else
		fprintf(stderr, "Error writing packet: %s\n", \
					libnet_geterror(l));

	/* Updating the offset */
	pload_offset += packet_pload_size;

	/* Clearing */
	/* We need to get rid of the ICMP header to build the other
	 * fragments */
	libnet_clear_packet(l);

	ip_tag = LIBNET_PTAG_INITIALIZER;

	/* Looping until all the payload is sent */
	while ( total_pload_size > pload_offset ) {
	
		/* Building IP header */

		/* checking if there will be more fragments */
		if ( (total_pload_size - pload_offset) > max_pload_size ) {
			/* In IP's eyes, the ICMP header in the first packet
			 * needs to be in the offset, so we add its size to the
			 * payload offset here */
			hdr_offset = IP_MF + (pload_offset + \
					LIBNET_ICMPV4_ECHO_H)/8;
			packet_pload_size = max_pload_size;
		}
		else {
			/* See above */
			hdr_offset = (pload_offset + LIBNET_ICMPV4_ECHO_H)/8;
			packet_pload_size = total_pload_size - pload_offset;
		}
	
		ip_tag = libnet_build_ipv4((LIBNET_IPV4_H + max_pload_size), \
				0, ip_id, hdr_offset, 255, IPPROTO_ICMP, 0, \
				src_addr, ip_addr, (payload + pload_offset), \
				packet_pload_size, l, ip_tag);

		if ( ip_tag == -1 ) {
			fprintf(stderr, "Error building IP header: %s\n", \
					libnet_geterror(l));
			libnet_destroy(l);
			exit(EXIT_FAILURE);
		}

		
		/* Writing packet */

		bytes_written = libnet_write(l);

		if ( bytes_written != -1 )
			printf("%d bytes written.\n", bytes_written);
		else
			fprintf(stderr, "Error writing packet: %s\n", \
					libnet_geterror(l));

		/* Updating the offset */
		pload_offset += packet_pload_size;
	}
}

u_int16_t get_sum(u_int8_t *payload, u_int32_t total_pload_size, \
		u_int16_t id, u_int16_t seq) {

	/* Builds the ICMP header with the whole payload, gets the checksum from
	 * it and returns it (in host order). */

	char errbuf[LIBNET_ERRBUF_SIZE];
	libnet_ptag_t icmp_tag;
	u_int8_t *packet;
	u_int32_t packet_size;
	u_int16_t *sum_p, sum;
	u_int8_t dummy_dst[6] = {0, 0, 0, 0, 0, 0};
	
	icmp_tag = LIBNET_PTAG_INITIALIZER;

	/* Switching to advanced link mode */
	/* Nothing should be built yet and all random numbers should be already
	 * generated. */
	libnet_destroy(l);
	l = libnet_init(LIBNET_LINK_ADV, NULL, errbuf);
	if ( l == NULL ) {
		fprintf(stderr, "libnet_init() failed (link_adv): %s\n", \
				errbuf);
		exit(EXIT_FAILURE);
	}

	/* Building the header */
	icmp_tag = libnet_build_icmpv4_echo(ICMP_ECHO, 0, 0, id, seq, \
			payload, total_pload_size, l, icmp_tag);
	
	if ( icmp_tag == -1 ) {

		fprintf(stderr, "Error building ICMP header: %s\n", \
				libnet_geterror(l));
		libnet_destroy(l);
		exit(EXIT_FAILURE);
	}

	/* Building dummy IP header */
	if ( libnet_autobuild_ipv4((LIBNET_IPV4_H + LIBNET_ICMPV4_ECHO_H + \
					total_pload_size), \
				IPPROTO_ICMP, 0, l) == -1 ) {
		fprintf(stderr,	"Error building dummy IP header: %s\n", \
				libnet_geterror(l));
		libnet_destroy(l);
		exit(EXIT_FAILURE);
	}

	/* Building dummy Ethernet header */
	if ( libnet_autobuild_ethernet (dummy_dst, ETHERTYPE_IP, l) == -1 ) {
		fprintf(stderr,	"Error building dummy Ethernet header: %s\n", \
				libnet_geterror(l));
		libnet_destroy(l);
		exit(EXIT_FAILURE);
	}


	/* Pulling the packet */
	if (libnet_adv_cull_packet(l, &packet, &packet_size) == -1) {
		fprintf(stderr, "Error pulling the packet: %s\n", \
				libnet_geterror(l));
		libnet_destroy(l);
		exit(EXIT_FAILURE);
	}

	/* Grabbing the checksum */
	/* We want the 37th and 38th bytes: eth header (14) + ip header (20) +
	 * icmp type and code (2) = 36 */
	sum_p = (u_int16_t*)(packet + 36); 
	sum = ntohs(*sum_p);

	/* Freeing memory */
	libnet_adv_free_packet(l, packet);

	/* Clearing the header */
	libnet_clear_packet(l);

	/* Switching back to IPv4 raw socket mode */
	libnet_destroy(l);
	l = libnet_init(LIBNET_RAW4, NULL, errbuf);
	if ( l == NULL ) {
		fprintf(stderr, "libnet_init() failed (raw4, 2nd call): %s\n", errbuf);
		exit(EXIT_FAILURE);
	}

	return sum;
}
/* EOF */
