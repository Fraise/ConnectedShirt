#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "http.h"
#include "teams.h"
#include <libcom.h>
#include <libthrd.h>

#define MAX_VALUES 40


/** Functions **/
void fillValeurs(FILE* client, FILE* webpage) {
	char buffer[MAX_BUFFER], filename[30], byte;
	int team = 0, cpt = 0, status;
	FILE* in = NULL;

	byte = fgetc(webpage);
	while ((byte != '<') && !feof(webpage) && (cpt < MAX_BUFFER)) {
		buffer[cpt] = byte;
		cpt++;
		byte = fgetc(webpage);
	}
	buffer[cpt] = '\0';

	if (sscanf(buffer, "team_%d", &team) == 1) {
		/* Getting last data saved in binary file */
		sprintf(filename, "./www/logs/team_%d.bin", team);
		P(FILE_MUTEX + team);
		in = fopen(filename, "rb");
		if (in != NULL) {
			Message* data = (Message*) malloc (sizeof(Message));
			Message* read = (Message*) malloc (sizeof(Message));
			status = fread(read, sizeof(Message), 1, in);
			while (status == 1) {
				memcpy(data, read, sizeof(Message));
				status = fread(read, sizeof(Message), 1, in);
			}
			free(read);
			fclose(in);

			/* Writing data to client */
			fprintf(client, "<td class=\"name\">%s</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td>", getTeamName(team), data->i, data->x, data->y, data->z, data->t);
			fflush(client);
			free(data);
		} else fprintf(stderr, "fillValeurs: Unable to open file %s\n", filename);
		V(FILE_MUTEX + team);
	} else fprintf(stderr, "fillValeurs: Bad request\n");
	fputc(byte, client); /* writes the < character */
}


void fillGraphes(FILE* client, FILE* webpage) {
	char buffer[MAX_BUFFER], filename[30], byte;
	int team = 0, cpt = 0, dataCnt = 0, status = 0, nbValues = 0, startIndex;
	FILE* in = NULL;

	byte = fgetc(webpage);
	while ((byte != ']') && !feof(webpage) && (cpt < MAX_BUFFER)) {
		buffer[cpt] = byte;
		cpt++;
		byte = fgetc(webpage);
	}
	buffer[cpt] = '\0';

	if (sscanf(buffer, "team_%d", &team) == 1) {
		/* Getting data saved in binary file */
		sprintf(filename, "./www/logs/team_%d.bin", team);
		P(FILE_MUTEX + team);
		in = fopen(filename, "rb");
		if (in != NULL) {
			/* Read the whole file to get number of values */
			Message* data = (Message*) malloc(sizeof(Message));
			status = fread(data, sizeof(Message), 1, in);
			while (status == 1) {
				nbValues++;
				status = fread(data, sizeof(Message), 1, in);
			}
			#ifdef DEBUG
				fprintf(stderr, "Successfully read %d objects from %s file\n", nbValues, filename);
			#endif

			/* Read the file again and write asked data */
			rewind(in);
			status = fread(data, sizeof(Message), 1, in);
			startIndex = (nbValues > MAX_VALUES) ? nbValues - MAX_VALUES : 0;
			while (status == 1) {
				if (dataCnt >= startIndex) {
					if (dataCnt > startIndex) fputc(',', client);
					fprintf(client, "{y:%ld000,a:%d,b:%d,c:%d,t:%d}", data->ts, data->x, data->y, data->z, data->t);
					fflush(client);
				}
				status = fread(data, sizeof(Message), 1, in);
				dataCnt++;
			}
			fclose(in);
		} else fprintf(stderr, "fillGraphes: Unable to open file %s\n", filename);
		V(FILE_MUTEX + team);
	} else fprintf(stderr, "fillGraphes: Bad request\n");

	fputc(byte, client); /* writes the ] character */
}


/** Main procedure **/
int createHttpClient(int socket) {
	char buffer[MAX_BUFFER];
	char cmd[MAX_BUFFER];
	char page[MAX_BUFFER];
	char proto[MAX_BUFFER];
	char path[MAX_BUFFER];
	char type[MAX_BUFFER];
	FILE* webpage = NULL;
	FILE* client = NULL;

	#ifdef DEBUG
		printf("Creating new http client on socket %d\n", socket);
	#endif

	client = fdopen(socket, "r+");
	if (client == NULL) {
		fprintf(stderr, "Failed to open the socket %d\n", socket);
		return -1;
	}

	if (fgets(buffer, MAX_BUFFER, client) == NULL) {
		fprintf(stderr, "Client connected on socket %d did not send any data\n", socket);
		return -1;
	}

	if (sscanf(buffer, "%s %s %s", cmd, page, proto) != 3) {
		fprintf(stderr, "Http request from client %d is not correctly formatted\n", socket);
		return -1;
	}

	while (fgets(buffer, MAX_BUFFER, client) != NULL)
		if (strcmp(buffer,"\r\n") == 0) break;

	if (strcmp(cmd, "GET") == 0) {
		if (strcmp(page, "/") == 0)
			sprintf(page, "/index.html");

		int code = CODE_OK;
		struct stat fstat;
		sprintf(path, "%s%s", WEB_DIR, page);
		if (stat(path, &fstat) != 0 || !S_ISREG(fstat.st_mode)) {
			sprintf(path, "%s/%s", WEB_DIR, PAGE_NOTFOUND);
			code = CODE_NOTFOUND;
		}
		strcpy(type, "text/html");
		char *end = page + strlen(page);
		if (strcmp(end - 4, ".png") == 0) strcpy(type, "image/png");
		if (strcmp(end - 4, ".jpg") == 0) strcpy(type, "image/jpg");
		if (strcmp(end - 4, ".gif") == 0) strcpy(type, "image/gif");
		fprintf(client, "HTTP/1.0 %d\r\n", code);
		fprintf(client, "Server: CWeb\r\n");
		fprintf(client, "Content-type: %s\r\n", type);
		fprintf(client, "Content-length: 30000\r\n");//, (int)fstat.st_size);
		fprintf(client, "\r\n");
		fflush(client);

		if (strcmp(page,"/valeurs.html") == 0) P(VALEURS_MUTEX);
		else if (strcmp(page,"/graphes.html") == 0) P(GRAPHES_MUTEX);
		webpage = fopen(path, "r");
		if (webpage != NULL) {
			unsigned char byte;
			byte = fgetc(webpage);
			while (!feof(webpage)) {
				if (byte == '$') {
					if (strcmp(page,"/valeurs.html") == 0) fillValeurs(client, webpage);
					else if (strcmp(page,"/graphes.html") == 0) fillGraphes(client, webpage);
				} else fputc(byte, client);
				byte = fgetc(webpage);
			}
			fclose(webpage);
			fprintf(client, "\r\n");
			fflush(client);
		} else {
			perror("createHttpClient.fopen webpage");
			return -1;
		}
		if (strcmp(page,"/valeurs.html") == 0) V(VALEURS_MUTEX);
		else if (strcmp(page,"/graphes.html") == 0) V(GRAPHES_MUTEX);
	} else fprintf(stderr, "Command not valid");

	#ifdef DEBUG
		printf("Http handling ended well, closing sock #%d\n", socket);
	#endif
	fclose(client);
	return 0;
}
