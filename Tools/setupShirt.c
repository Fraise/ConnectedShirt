#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <libcom.h>


void usage() {
	printf("Usage: setupShirt [-p port] [-i ip_address] [-d destination_id] [-f period(cs) | -n new_id]\n");
	printf("Presets: -p 54321 -i 172.26.79.215 -d 00\n");
}


int main(int argc, char** argv) {
	/* Analyzing options */
	int option = 0, status, i;
	char *port = "54321", *ip = "172.26.79.215";
	char *new_team = "00", *dest_team = "00";
	char *frq = "100";
	int i_frq, i_dest_team, i_new_team;
	int fflag = 0, dflag = 0, nflag = 0;

	/* Getting options */
	while((option = getopt(argc, argv, "p:i:f:d:n:")) != -1) {
		switch(option) {
			case 'p':
				port = optarg;
				break;
			case 'i':
				ip = optarg;
				break;
			case 'f':
				if (nflag) { usage(); return -1; }
				else { frq = optarg; fflag = 1; }
				break;
			case 'd':
				dest_team = optarg;
				dflag = 1;
				break;
			case 'n':
				if (fflag) { usage(); return -1; }
				else { new_team = optarg; nflag = 1; }
				break;
			case ':':
				fprintf(stderr, "Option -%c needs an argument\n", optopt);
				return -1;
				break;
			case '?':
				fprintf(stderr, "Unrecognized option -%c\n", optopt);
				return -1;
			default:
				fprintf(stderr, "bblblblblbll\n");
				return -1;
		}
	}

	if (!(dflag || nflag)) { usage(); return -1; }

	/* Printing some info */
	sscanf(dest_team, "%d", &i_dest_team);
	printf("Sending udp packet to %s via port %s with parameters:\n", ip, port);
	printf("> Destination team id: %d\n", i_dest_team);
	if (fflag) {
		sscanf(frq, "%d", &i_frq);
		printf("> New period: %d0 ms\n", i_frq);
	} else {
		sscanf(new_team, "%d", &i_new_team);
		printf("> New team id: %d\n", i_new_team);
	}

	/* Sending the packet */
	unsigned char message[5] = { ((unsigned char) i_dest_team << 4), 0, 0, 0, 0 };
	if (!fflag) message[1] = (unsigned char) i_new_team;
	else message[4] = (i_frq < 255) ? (unsigned char) i_frq : 255;
	for (i = 0; i < 5; i++) {
		status = envoiMessageUnicast(ip, port, message, 5);
		if (status < 0) { fprintf(stderr, "envoiMessageUnicast.error\n"); return -1; }
		usleep(200000);
	}
	return 0;
}
