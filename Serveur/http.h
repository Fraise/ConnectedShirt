#ifndef __HTTP_H__
#define __HTTP_H__

/** Some constants **/
#define WEB_DIR  "./www"
#define PAGE_NOTFOUND "error.html"
#define MAX_BUFFER 1024

#define CODE_OK  200
#define CODE_NOTFOUND 404

/** Functions **/
void fillValeurs(FILE* client, FILE* webpage);
void fillGraphes(FILE* client, FILE* webpage);
int createHttpClient(int socket);

#endif
