/** fichier libcom.h **/
/** Ce fichier decrit les structures et les constantes utilisees **/
/** par les fonctions de gestion des sockets                     **/

#ifndef __LIBCOM_H__
#define __LIBCOM_H__

/**** Constantes ****/
#define UDP_BUFSIZE 5
#define MAX_CONNEXIONS 16

void stopServers();

/**** Fonctions pour le serveur TCP ****/
int initialisationServeur(char *service, int connexions);
int boucleServeur(int ecoute, void (*traitement)(int));

/**** Fonctions pour le serveur UDP ****/
int envoiMessage(char* port, unsigned char* message, int taille);
int envoiMessageUnicast(char *hote, char *service, unsigned char *message, int taille); // Fonction d'envoi de message par UDP, hote: @serveur, service: port
int initialisationSocketUDP(char *service); // Fonction d'initialisation de serveur UDP, service: port
int boucleServeurUDP(int s, void (*traitement)(unsigned char *, int)); // Fonction de boucle serveur, message traite par fct traitement
void serveurMessages(char *port, void (*traitement)(unsigned char *, int)); // Fonction de demarrage de serveur UDP et d'ecoute
void serveurTCP(char *port, void (*traitement)(int)); // Fonction de demarrage de serveur TCP

#endif
