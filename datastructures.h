#include "globaldefs.h"

typedef struct label {
	char name[MAXSTRLEN];
	int address;
	struct label *next;
} Label;

