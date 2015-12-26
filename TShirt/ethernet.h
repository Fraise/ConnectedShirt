/** Definitions pour le forgeage de paquet **/

#ifndef __PACKET_H__
#define __PACKET_H__

#define TEAM_ID 7
#define DATA_LENGTH 4
#define UDP_SRC_PORT    12007
#define UDP_DEST_PORT   12345

extern const unsigned char skel_packet[33];

void build_packet(unsigned char* data, unsigned char* packet);
void do_ip_cksm(unsigned char* packet);
void do_udp_cksm(unsigned char* packet);
void do_parity(unsigned char* packet);
void set_id(unsigned char lol);
unsigned char get_id(void);

#endif
