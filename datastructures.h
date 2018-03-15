#include "globaldefs.h"

typedef enum labelType {
        data=1,
        command=2,
} LabelType;


typedef struct label {
	char name[MAXSTRLEN];
	int address;
	LabelType type;
	struct label *next;
} Label;

int pushLabel(Label** head, char *newLabelName, int newLabelAddress, LabelType lType);
int getAddressByName(Label **head, char *nameToSearch);
int safePushLabel(Label **head, char *newLabelName, int newLabelAddress, LabelType lType);
void printLabels(Label **head);
int updateDataLabels(Label **head, int IC);
