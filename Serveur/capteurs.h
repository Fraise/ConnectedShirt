#ifndef __CAPTEURS_H__
#define __CAPTEURS_H__

/** Structures definitions **/
typedef struct udpParameters {
    int size;
    unsigned char packet[1];
} UDPParameters;

bool check_parity(unsigned char* packet);
void saveData(unsigned char* packet, int size);

#endif
