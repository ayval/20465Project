#include "globaldefs.h"

typedef struct label {
	char name[MAXSTRLEN];
	int address;
	struct label *next;
} Label;

int pushLabel(Label** head, char *newLabelName, int newLabelAddress);
int getAddressByName(Label **head, char *nameToSearch);
int safePushLabel(Label **head, char *newLabelName, int newLabelAddress);
void printLabels(Label **head);
