#ifndef __SLIP_H__
#define __SLIP_H__

/* SLIP special character codes */
#define END             192 //0300    /* indicates end of packet */
#define ESC             219 //0333    /* indicates byte stuffing */
#define ESC_END         220 //0334    /* ESC ESC_END means END data byte */
#define ESC_ESC         221 //0335    /* ESC ESC_ESC means ESC data byte */
 
void slip_send_packet(unsigned char *p, int len);
int slip_recv_packet(unsigned char *p, int len);

#endif
