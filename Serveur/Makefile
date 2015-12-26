# Makefile pour generer le serveur 

OBJS = serveur.o http.o capteurs.o teams.o
LIBS = -lpthread
LOCAL_LIBS  = -L ../Communication -L ../Threads
LOCAL_LIBS += -lcom -lthrd
CFLAGS     += -I ../Communication -I ../Threads

# Cible generale
all: serveur

# La cible de nettoyage
clean: 
	rm -f core *.o serveur http

# Les cibles pour l'executable
serveur: $(OBJS)
	$(CC) $(CFLAGS) -o serveur $(OBJS) $(LOCAL_LIBS) $(LIBS)

serveur.o: serveur.c
http.o: http.c http.h
capteurs.o: capteurs.c capteurs.h
teams.o: teams.c teams.h
