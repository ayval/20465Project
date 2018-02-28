#include<stdio.h>
#include "globaldefs.h"
#include "fileparserutils.h"
#include "datastructures.h"
#include <string.h>

int firstPass(FILE *aFile , Label **labelsList) { 

	/*internal function variables*/	
	Directive mDirective;
	char line[MAXSTRLEN];
	char token[MAXSTRLEN];
	char mLabelName[MAXSTRLEN];
	int lineDelta=0;;
	int tempLineDelta=0;
	char tempString[MAXSTRLEN];
	/*flags*/
	/*Flag that indicates a label*/
	int labelFlag = FALSE;
	/*Flag that indicates if there was an error so we can exit at the end of the while loop*/
	int errorFlag = FALSE;
	
	/*stage 1 - init instruction ocunter and data counter*/
	int IC=0;
	int DC=0;
	int L=0;
	/*stage 2 - read next line from file*/
	while (fgets(line, MAXSTRLEN, aFile)) {
		lineDelta=0;
		printf("DC=%d IC=%d\n",DC,IC);
		printf("---------------------\n %s---------------------\n",line);
		/*stage 3 - is the first item a label?*/
		tempLineDelta = getNextToken(token,line);
		if (isLabel(token)) {
			lineDelta+=tempLineDelta;
			labelFlag=TRUE;
			strcpy(mLabelName, token);
			printf("Label=%s\n",mLabelName);

			/*stage 4 - check the label flag and move to the next token*/
			labelFlag = TRUE;
			tempLineDelta = getNextToken(token,line+lineDelta);
		}
		/*stage 5 - check if this is a directive*/
		mDirective = getDirective(token);
		if (mDirective) {
			lineDelta+=tempLineDelta;
			if (mDirective==DataDirective || mDirective==StringDirective || mDirective==StructDirective) {
				printf("This is a directive: %s of type:%d\n",token, mDirective);
				/*stage 6 - if there is a label, add it to the list*/
				if (labelFlag) {
					/*update error flag according to result of PushLabel*/
					errorFlag = ! safePushLabel(labelsList, mLabelName, DC);
				}
			/*stage 7 - calculate data length and update update DC*/
				printf("Data to parse: \"%s\"\n",line+lineDelta);
				strcpy(tempString, line+lineDelta);
				if (mDirective==DataDirective) {
					printf("Going to analyze the data\n");
					DC+=getDataLength(tempString);
				}
			}
			/*stage 8*/
			if (mDirective==ExternalDirective) {
				/*stage 9 - do something*/
			}
			/*stage 10 - go back to the next line*/
			continue;	
		}
		/*stage 11*/
		if (labelFlag) {
			errorFlag = ! safePushLabel(labelsList, mLabelName,IC);
		}
		/*stage 12+13*/
		/*analyze rest of data*/
		
		printf("Command to parse: %s\n",line+lineDelta);
		
		/* stage - 15 go back to the next line*/
		/*zero the relevant flags*/
		labelFlag=FALSE;
		if (errorFlag) {
			break;
		}	
	}
}


int main() {
	Label *mLabel=NULL;
	FILE *f = fopen("testdata1.txt","r");
	if (! f) {
		printf("The file is null\n");
	}
	firstPass(f, &mLabel);
	return 0;
}
