#include "globaldefs.h"

#ifndef DATASTRUCTURES_INC
#define DATASTRUCTURES_INC

typedef struct aStruct {
	char data[MAXMEMORY];
	int number;
	int length;
} AStruct;

typedef struct aData {
	int data[80];
	int length;
} AData;


typedef struct label {
	char name[MAXSTRLEN];
	int address;
	LabelType type;
	AData data;
	AStruct aStruct;
	struct label *next;
} Label;

int pushLabel(Label** head, char *newLabelName, int newLabelAddress, LabelType lType);
int getAddressByName(Label **head, char *nameToSearch);
int getTypeByName(Label **head, char *nameToSearch);
int safePushLabel(Label **head, char *newLabelName, int newLabelAddress, LabelType lType);
void printLabels(Label **head);
int updateDataCounterLabels(Label **head, int IC);
int updateLabelData(Label **head, AData *aData, char *labelName);
int updateLabelString(Label **head, char *stringValue, char *labelName);
int updateLabelStruct(Label **head, AStruct *analyzedStruct, char *labelName);
int freeLabels(Label **head);



#endif /*DATASTRUCTURES_INC*/
