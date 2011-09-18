/* libnet 1.1 tutorial (https://repolinux.wordpress.com/category/libnet/) sample code
 * Example 10: cull_packet.c */

#include <stdio.h>
#include <stdlib.h>
#include <libnet.h>
#include <stdint.h>

int main() {

	/* Builds an ARP request, then pulls it from libnet, changes the target
	 * IP address and writes it. */

	libnet_t *l;
	char errbuf[LIBNET_ERRBUF_SIZE], target_ip_addr_str[16];
	u_int32_t src_ip_addr, target_ip_addr = 0;
	u_int8_t mac_broadcast_addr[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
		 mac_zero_addr[6] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
	struct libnet_ether_addr *src_mac_addr;
	int i;

	u_int8_t *packet, *target_ip_addr_p;
	u_int32_t packet_size;


	l = libnet_init(LIBNET_LINK_ADV, NULL, errbuf);
	if ( l == NULL ) {
		fprintf(stderr, "libnet_init() failed: %s\n", errbuf);
		exit(EXIT_FAILURE);
	}

	/* Getting our own MAC and IP addresses */
	src_ip_addr = libnet_get_ipaddr4(l);
	if ( src_ip_addr == -1 ) {
		fprintf(stderr, "Couldn't get own IP address: %s\n", libnet_geterror(l));
		libnet_destroy(l);
		exit(EXIT_FAILURE);
	}

	src_mac_addr = libnet_get_hwaddr(l);
	if ( src_mac_addr == NULL ) {
		fprintf(stderr, "Couldn't get own IP address: %s\n", libnet_geterror(l));
		libnet_destroy(l);
		exit(EXIT_FAILURE);
	}

	/* Building ARP header with target IP address = 0.0.0.0 */
	if ( libnet_autobuild_arp (ARPOP_REQUEST,\
			src_mac_addr->ether_addr_octet,\
			(u_int8_t*)(&src_ip_addr), mac_zero_addr,\
			(u_int8_t*)(&target_ip_addr), l) == -1)
	{
		fprintf(stderr, "Error building ARP header: %s\n",\
				libnet_geterror(l));
		libnet_destroy(l);
		exit(EXIT_FAILURE);
	}
	
	/* Building Ethernet header */
	if ( libnet_autobuild_ethernet (mac_broadcast_addr, ETHERTYPE_ARP, l)\
			== -1 )
	{
		fprintf(stderr, "Error building Ethernet header: %s\n",\
				libnet_geterror(l));
		libnet_destroy(l);
		exit(EXIT_FAILURE);
	}

	/* Pulling the packet */
	if (libnet_adv_cull_packet(l, &packet, &packet_size) == -1) {
		fprintf(stderr, "libnet_adv_cull_packet() failed: %s\n",\
				libnet_geterror(l));
		libnet_destroy(l);
		exit(EXIT_FAILURE);
	}

	/* Getting target IP address */
	printf("Target IP address: ");
	scanf("%15s",target_ip_addr_str);

	target_ip_addr = libnet_name2addr4(l, target_ip_addr_str,\
			LIBNET_DONT_RESOLVE);

	if ( target_ip_addr == -1 ) {
		fprintf(stderr, "Error converting IP address.\n");
		libnet_destroy(l);
		exit(EXIT_FAILURE);
	}

	/* Changing the target */
	/* We want to change the 39th, 40th, 41st and 42nd bytes:
	 * Ethernet header (14) + ARP's hw(2), proto(2), hw size(1), proto 
	 * size(1), opcode(2), src hw addr(6), src ip add(4), target hw addr(6)
	 * = 38 */
	if (packet_size >= 42) {
		target_ip_addr_p = (u_int8_t *)&target_ip_addr;
		for (i=0; i < 4; i++) {
			packet[38+i] = target_ip_addr_p[i];
		}
	}

	/* Writing packet */
	if (libnet_adv_write_link(l, packet, packet_size) == -1) {
		fprintf(stderr, "Error writing packet: %s\n",\
				libnet_geterror(l));
	}

	/* Freeing up the memory */
	libnet_adv_free_packet(l, packet);
	
	libnet_destroy(l);
	return 0;
}
/* EOF */
