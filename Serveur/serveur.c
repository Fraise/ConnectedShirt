/** Fichier serveur.c **/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <libcom.h>
#include <libthrd.h>
#include "http.h"
#include "serveur.h"
#include "capteurs.h"

#define TIMEOUT 30

/** Global variables **/
static bool _stop = false;
static struct sigaction action;


int main(int argc, char *argv[]) {
	/* Analyzing options */
	int option = 0;
	char* portUDP = "12345";
	char* portTCP = "4200";
	/* Getting options */
	while((option = getopt(argc, argv, "p:")) != -1) {
		if (option == 'p') portUDP = optarg;
		else fprintf(stderr, "Unrecognized option, using default UDP port: %s\n", portUDP);
	}

	/* Signal handling initialization */
	action.sa_handler = hand;
	sigaction(SIGINT, &action, NULL);

	/* Mutexes initialization */
	initMutexes(13, 1);
	/* Starting UDP messages server */
	lanceThread(startUDPServer, (void*) portUDP, sizeof(portUDP));
	/* Starting TCP server */
	lanceThread(startTCPServer, (void*) portTCP, sizeof(portTCP));

	/* Main process sleeping while servers are working */
	while (!_stop) sleep(1);

	/* Waiting for threads to terminate */
	int timeout = 0;
	while (getLivingThreads() != 0 && timeout < TIMEOUT) { sleep(1); timeout++; }
	if (timeout == TIMEOUT) printf("Servers quit: timeout\n");

	/* Freeing the mutexes */
	sem_free();

	return 0;
}


/* TCP functions */
void threadedTraitementTCP (void* arg) {
	int sock = *((int *)arg);
	#ifdef DEBUG
		fprintf(stderr, "Started new TCP process thread on sock #%d\n", sock);
	#endif
	createHttpClient(sock);
}


void traitementTCP (int sock) {
	int tmp = sock;
	if (lanceThread(&threadedTraitementTCP, (void *) &tmp, sizeof(int)) < 0) {
		perror("traitementTCP.lanceThread"); exit(EXIT_FAILURE);
	}
}


void startTCPServer(void* arg) {
	serveurTCP((char*)arg, &traitementTCP);
}

/* UDP functions */
void threadedTraitementUDP(void* arg) {
	UDPParameters* param = arg;
	#ifdef DEBUG
		fprintf(stderr, "Started new UDP process thread, packet size: %d\n", param->size);
	#endif
	saveData(param->packet, param->size);
}

void traitementUDP(unsigned char* packet, int size) {
	int allocated = sizeof(UDPParameters) + size - 1;
	UDPParameters* param = (UDPParameters*) malloc(allocated);
	memcpy(param->packet, packet, size);
	param->size = size;
	if (lanceThread(&threadedTraitementUDP, (void *) param, allocated) < 0) {
		perror("traitementUDP.lanceThread"); exit(EXIT_FAILURE);
	}
	free(param);
}

void startUDPServer(void* arg) {
	serveurMessages((char*)arg, &traitementUDP);
}

/* Signals handling function */
void hand(int sig) {
	if (sig == SIGINT) {
		printf("SIGINT signal received, closing servers...\n");
		stopServers();
		_stop = true;
	} else perror("Unrecognized signal received");
}
