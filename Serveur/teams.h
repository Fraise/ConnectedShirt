#ifndef __TEAMS_H__
#define __TEAMS_H__

#define NB_TEAMS 11

typedef struct message {
	unsigned char i;
	unsigned char x;
	unsigned char y;
	unsigned char z;
	unsigned char t;
	long int ts;
} Message;

char* getTeamName(int i);


#endif
