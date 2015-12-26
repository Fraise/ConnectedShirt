#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <time.h>

#include <libcom.h>

#define MAX_TEAMS 11

void usage() {
	printf("Usage: flooder [-p port] [-i ip_address] [ -t time (ms) ]\n");
	printf("Presets: -p 12345 -i 127.0.0.1 -t 50\n");
}


int main(int argc, char** argv) {
	/* Analyzing options */
	int option = 0, team;
	char *port = "12345", *ip = "127.0.0.1", *frq = "50";
  int i_frq = 0;

	/* Getting options */
	while((option = getopt(argc, argv, "p:i:f:")) != -1) {
		switch(option) {
			case 'p':
				port = optarg;
				break;
			case 'i':
				ip = optarg;
				break;
			case 'f':
				frq = optarg;
				break;
			case '?':
				fprintf(stderr, "Unrecognized option -%c\n", optopt);
				return -1;
			default:
				fprintf(stderr, "argument error\n");
				return -1;
		}
	}

  if (sscanf(frq, "%d", &i_frq) < 0) { usage(); return 1; }
  srand(time(NULL));

	/* Printing some info */
	printf("Starting udp flooding to %s via port %s at %dms\n", ip, port, i_frq);

  unsigned char message[5] = { 0, 0, 0, 0, 0 };
	/* Sending the packet */
	do {
    team = rand() % MAX_TEAMS;
    message[0] = ((unsigned char) team << 4) | 0x0F;
		envoiMessageUnicast(ip, port, message, 5);
		usleep(i_frq * 1000);
	} while (1);
	return 0;
}
