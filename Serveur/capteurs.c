#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

#include <libthrd.h>
#include <libcom.h>
#include "capteurs.h"
#include "http.h"
#include "teams.h"

#define LED_ACK_FRQ 5


bool check_parity(unsigned char* packet) {
	int i = 1, j;
	bool res = true;
	unsigned char data, b = 1, tmp;
	do {
		data = packet[i];
		tmp = 1;
		for (j = 0; j < 8; j++) if (data & (b << j)) tmp++;
		tmp = tmp % 2;
		res &= ((packet[0] >> (4 - i)) & 0x01) == tmp;
		i++;
	} while (res && i < 5);
	return res;
}


void saveData(unsigned char* packet, int size) {
	if (size == 5 && check_parity(packet)) {
		int team = (int) ((packet[0] & 0xF0) >> 4), i;
		Message* data = (Message*) malloc(sizeof(Message));
		/* Putting data in structure */
		data->i = team;
		data->x = packet[1];
		data->y = packet[2];
		data->z = packet[3];
		data->t = packet[4];
		data->ts = (long int) time(NULL);

		/* Detecting fall and sending ack */
		if ((packet[0] & 0x0F) == 0x0F && packet[1] == 0xFF && packet[2] == 0xFF && packet[3] == 0xFF && packet[4] == 0xFF) {
			unsigned char ack[5] = { ((unsigned char) team << 4), 0, 0, 0, LED_ACK_FRQ };
			#ifdef DEBUG
				fprintf(stderr, "Sending fall ack to team %d\n", team);
			#endif
			for (i = 0; i < 5; i++) {
				envoiMessage("54321", ack, 5);
				usleep(300000);
			}
		}

		/* Saving data in binary file */
		char filename[30];
		sprintf(filename, "./www/logs/team_%d.bin", team);
		P(FILE_MUTEX + team);
		FILE* out = fopen(filename, "ab");
		if (out == NULL) { fprintf(stderr, "saveData: unable to open file %s. Does the folder exist?\n", filename); return; }
		fwrite(data, sizeof(Message), 1, out);
		fclose(out);
		V(FILE_MUTEX + team);
		free(data);
		/* Finished */
	} else {
		fprintf(stderr, "Received invalid packet (wrong size/parity)\n");
	}
	#ifdef DEBUG
		fprintf(stderr, "Finished processing UDP packet\n");
	#endif
}
