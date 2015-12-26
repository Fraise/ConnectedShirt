#include <stdlib.h>
#include <stdio.h>

#include "teams.h"

static char teamsName[NB_TEAMS][25] = {"Jean & Flavien",
	"Cyril & JM",
	"Kevin & Benjamin",
	"Valentin & Alexander",
	"Timothee & Mageshwaran",
	"Jeremie & Julien",
	"Mehdi & Thibault",
	"Romain & Alexandre",
	"Sandra & Elise",
	"Hideo & Jerome",
	"Arnaud"
};

char* getTeamName(int i) {
	return teamsName[i];
}
