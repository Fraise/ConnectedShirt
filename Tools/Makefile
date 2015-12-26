OBJS = setupShirt.o flooder.o
LOCAL_LIBS  = -L ../Communication
LOCAL_LIBS += -lcom
CFLAGS     += -I ../Communication

# Cible generale
all: setupShirt flooder

# La cible de nettoyage
clean:
	rm -f core *.o setupShirt flooder

# Les cibles pour l'executable
setupShirt: $(OBJS)
	$(CC) $(CFLAGS) -o setupShirt setupShirt.o $(LOCAL_LIBS)

flooder: $(OBJS)
	$(CC) $(CFLAGS) -o flooder flooder.o $(LOCAL_LIBS)

setupShirt.o: setupShirt.c
flooder.o: flooder.c
