LDFLAGS = -Wl,--no-as-needed `libnet-config --libs`
CFLAGS = -ggdb -Wall `libnet-config --defines`

OBJECTS = 01_init 02_init_devname 03_addr 04_get_own_addr 05_ping 06_arp 07_reping_tags 08_reping_clear 09_cull_header 10_cull_packet 11_frag_ping 12_addr6 13_ping6

all : $(OBJECTS)
clean :
	\rm $(OBJECTS)
.PHONY: all clean

% : %.c
