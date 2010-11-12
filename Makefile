LDFLAGS = `libnet-config --libs`
CFLAGS = -ggdb -Wall `libnet-config --defines`

OBJECTS = 1.1_init 1.2_init_devname 1.3_addr 1.4_get_own_addr 2.1_ping 2.2_arp 2.3_reping_tags 2.4_reping_clear 3.1_cull_header 3.2_cull_packet 3.3_frag_ping

all : $(OBJECTS)
clean :
	\rm $(OBJECTS)
.PHONY: all clean

% : %.c
