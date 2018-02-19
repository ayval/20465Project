#include<stdio.h>
#include "globaldefs.h"
#include "fileparserutils.h"
#include <string.h>

int firstPass(FILE *aFile) { 
	
	char *token;
	char line[MAXSTRLEN];
	
	/*stage 1 - init instruction ocunter and data counter*/
	int IC=0;
	int DC=0;

	/*stage 2 - read next line from file*/
	fgets(line, MAXSTRLEN, aFile);

	/*stage 3 - is the first item a label?*/
	token = strtok(line, WHITESPACE);
	printf("This is the first token: %s\n",token);
	return 1;
}



int main() {
	FILE *f = fopen("testdata1.txt","r");
	if (! f) {
		printf("The file is null\n");
	}
	firstPass(f);
	return 0;
}
