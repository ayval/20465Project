#include <stdio.h>
#include "globaldefs.h"
#include "fileparserutils.h"
#include "datastructures.h"
#include "stringparserutils.h"
#include <string.h>
#include <stdlib.h>

int firstPass(FILE *aFile , Label **labelsList, Command *commandList[]) { 

	/*internal function variables*/	
	Directive mDirective;
	char line[MAXSTRLEN];
	char token[MAXSTRLEN];
	char mLabelName[MAXSTRLEN];
	int lineDelta=0;
	int tempLineDelta=0;
	int lineNumber=0;
	int commandCounter=0;
	AStruct analyzedStruct;
	AData analyzedData;
	char tempString[MAXSTRLEN];
	char operandsToParse[MAXSTRLEN];
	Command *analyzedCommand;
	/*flags*/
	/*Flag that indicates a label*/
	int labelFlag = FALSE;
	/*Flag that indicates if there was an error so we can exit at the end of the while loop*/
	int errorFlag = FALSE;
	
	/*stage 1 - init instruction ocunter and data counter*/
	int IC=0;
	int DC=0;
	/*int L=0;*/
	/*stage 2 - read next line from file*/
	while (fgets(line, MAXSTRLEN, aFile)) {
		lineDelta=0;
		lineNumber++;
		if (isComment(line))
			continue;
		if(isEmpty(line))
			continue;
		printf("DC=%d IC=%d\n",DC,IC);
		printf("-----------------------------------------------------\n%s",line);
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
				printf("Directive=%s of type:%d ",token, mDirective);
				/*stage 6 - if there is a label, add it to the list*/
				if (labelFlag) {
					/*update error flag according to result of PushLabel*/
					errorFlag = ! safePushLabel(labelsList, mLabelName, DC, data);
				}
			/*stage 7 - calculate data length and update update DC*/
				printf("Operands: %s",line+lineDelta);
				strcpy(operandsToParse, line+lineDelta);
				if (mDirective==DataDirective) {
					errorFlag = ! getData(&analyzedData, operandsToParse);
					DC+=analyzedData.length;
				}
				if (mDirective==StringDirective) {
					if (! cleanString(tempString,operandsToParse)) {
						errorFlag = TRUE;
					}
					DC+=strlen(tempString);
					DC++; /*to count the string terminator*/
				}
				if (mDirective==StructDirective) {
					errorFlag = getStruct(tempString, analyzedStruct);
					DC+=analyzedStruct.length;
				}
				if (errorFlag) {
					printf("Error: Aborting current parsing at line %d.Illegal directive operands: %s\n",lineNumber,operandsToParse );
					break;
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
			errorFlag = ! safePushLabel(labelsList, mLabelName,IC,command);
		}
		/*stage 12+13*/
		/*analyze rest of data*/
		analyzedCommand=(Command*)malloc(sizeof(Command));
		printf("Command to parse: %s\n",line+lineDelta);
		getCommand(line+lineDelta, analyzedCommand);
		enrichCommand(analyzedCommand);
		analyzedCommand->IC=IC;
		printCommand(analyzedCommand);
		commandList[commandCounter]=analyzedCommand;
		commandCounter++;
		IC=IC+1+getAdditionalWordCount(analyzedCommand);
		/* stage - 15 go back to the next line*/
		/*zero the relevant flags*/
		labelFlag=FALSE;
		if (errorFlag) {
			break;
		}
		errorFlag=FALSE;
	}
	/*fix the data labels with IC*/
	if (! errorFlag) {
		 printf("update data labels here\n");
		 updateDataLabels(labelsList, IC);

	}
	return errorFlag;
}

int parseCommands(Command *commandList[], char binaryCodes[][11]) {
	char binaryString[50];
	for (int i=0; i<256; i++) {
		printf("i=%d\n",i);
		if (commandList[i]) {
			memset(binaryString,'\0',50);
			printf("operation:%d\n",commandList[i]->op);
			commandToBin(binaryString, commandList[i]);
			/*copy the command to the command list with its operands. 
			+j/10 pushes the counter to the next word if needed*/
			printf("before for\n");
			for (int j=0; j<strlen(binaryString); j++) {
				binaryCodes[commandList[i]->IC + j/10][j % 10] = binaryString[j];
			}
		}
	}
	return TRUE;
}


int main() {
	Label *mLabel=NULL;
	Command *commandList[256];
	char binaryCodes[256][11];
	memset(binaryCodes,'\0',256*10);
	for (int i=0; i<256; i++)
		commandList[i]=NULL;
	FILE *f = fopen("testdata1.txt","r");
	if (! f) {
		printf("The file is null\n");
	}
	firstPass(f, &mLabel, commandList);
	parseCommands(commandList, binaryCodes);
	printf("after parse commands\n");
	printLabels(&mLabel);
	for (int i=0; i<30; i++)
		printf("bin command: %s\n",binaryCodes[i]);
	return 0;
}
