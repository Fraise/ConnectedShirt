#ifndef __SERVEUR_H__
#define __SERVEUR_H__

void threadedTraitementTCP (void* arg);
void traitementTCP (int sock);
void startTCPServer(void* arg);

void traitementUDP(unsigned char* packet, int size);
void startUDPServer(void* arg);
void threadedTraitementUDP(void* arg);


void hand(int sig);

#endif
