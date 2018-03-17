#include <stdio.h>
#include "globaldefs.h"
#include "fileparserutils.h"
#include "datastructures.h"
#include "stringparserutils.h"
#include <string.h>
#include <stdlib.h>

int firstPass(FILE *aFile , Label **labelsList, Command *commandList[]) { 

	/*internal function variables*/	
	LabelType mDirective;
	char line[MAXSTRLEN];
	char token[MAXSTRLEN];
	char mLabelName[MAXSTRLEN];
	char mCleanLabelName[MAXSTRLEN];
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
		/*printf("DC=%d IC=%d\n",DC,IC);
		printf("-----------------------------------------------------\n%s",line);*/
		/*stage 3 - is the first item a label?*/
		tempLineDelta = getNextToken(token,line);
		if (isLabel(token)) {
			lineDelta+=tempLineDelta;
			labelFlag=TRUE;
			strcpy(mLabelName, token);
			/*printf("Label=%s\n",mLabelName);*/

			/*stage 4 - check the label flag and move to the next token*/
			labelFlag = TRUE;
			tempLineDelta = getNextToken(token,line+lineDelta);
		}
		/*stage 5 - check if this is a directive*/
		mDirective = getDirective(token);
		if (mDirective) {
			lineDelta+=tempLineDelta;
			if (mDirective==data || mDirective==string || mDirective==structlabel) {
				/*printf("Directive=%s of type:%d ",token, mDirective);*/
				/*stage 6 - if there is a label, add it to the list*/
				if (labelFlag) {
					/*update error flag according to result of PushLabel*/
					cleanLabel(mCleanLabelName, mLabelName);
					errorFlag = ! safePushLabel(labelsList, mCleanLabelName, DC, mDirective);
				}
			/*stage 7 - calculate data length and update update DC*/
				/*printf("Operands: %s",line+lineDelta);*/
				strcpy(operandsToParse, line+lineDelta);
				if (mDirective==data) {
					/*TODO add better error handling*/
					errorFlag = ! getData(&analyzedData, operandsToParse);
					errorFlag += ! updateLabelData(labelsList, &analyzedData, mCleanLabelName);
					DC+=analyzedData.length;
				}
				if (mDirective==string) {
					errorFlag = ! cleanString(tempString, operandsToParse);
					errorFlag += ! updateLabelString(labelsList, tempString, mCleanLabelName);
					DC+=strlen(tempString);
					DC++; /*to count the string terminator*/

				}
				if (mDirective==structlabel) {
					errorFlag = getStruct(tempString, analyzedStruct);
					updateLabelStruct(labelsList, &analyzedStruct, mCleanLabelName);
					DC+=analyzedStruct.length;
				}
				if (errorFlag) {
					printf("ERROR: Aborting current parsing at line %d.Illegal directive operands: %s\n",lineNumber,operandsToParse );
					break;
				}
			}
			/*stage 8*/
			if (mDirective==external) {
				/*stage 9 - do something*/
			}
			/*stage 10 - go back to the next line*/
			continue;	
		}
		/*stage 11*/
		if (labelFlag) {
			cleanLabel(mCleanLabelName, mLabelName);
			errorFlag = ! safePushLabel(labelsList, mCleanLabelName,IC,command);
		}
		/*stage 12+13*/
		/*analyze rest of data*/
		analyzedCommand=(Command*)malloc(sizeof(Command));
		getCommand(line+lineDelta, analyzedCommand);
		enrichCommand(analyzedCommand);
		analyzedCommand->IC=IC;
		analyzedCommand->codeLine=lineNumber;
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
		 updateDataCounterLabels(labelsList, IC);

	}
	return errorFlag;
}

int parseCommands(Command *commandList[], char binaryCodes[][MAXBITSINMUZAR+1], Label *labels) {
	char binaryString[MAXBITSINMUZAR*5];
	int i,j;
	for (i=0; i<256; i++) {
		if (commandList[i]) {
			memset(binaryString,'\0',MAXBITSINMUZAR*5);
			commandToBin(binaryString, commandList[i], labels);
			/*copy the command to the command list with its operands. 
			+j/10 pushes the counter to the next word if needed*/
			for (j=0; j<strlen(binaryString); j++) {
				binaryCodes[commandList[i]->IC + j/MAXBITSINMUZAR][j % MAXBITSINMUZAR] = binaryString[j];
			}
		}
	}
	return TRUE;
}

int parseLabels(char binaryCodes[][MAXBITSINMUZAR+1], Label *labels) {
	char binaryString[MAXBITSINMUZAR*MAXMEMORY];
	int j;
	memset(binaryString, '\0', MAXBITSINMUZAR*MAXMEMORY);
	Label *temp=labels;
	while (temp) {
		/*parse the data from the label struct*/
		memset(binaryString, '\0', MAXBITSINMUZAR*MAXMEMORY);
		if (temp->type==data) {
			getBinData(binaryString, &temp->data);
		}
		if (temp->type==string) {
			getBinString(binaryString, temp->aStruct.data);
		}
		if (temp->type==structlabel) {
			getBinStruct(binaryString, &temp->aStruct);
			printf("struct bin: %s\n", binaryString);
		}
		/*add the parsed binary data to the binary array representing the memory*/
		for (j=0; j<strlen(binaryString); j++) {
			binaryCodes[temp->address + j/MAXBITSINMUZAR][j % MAXBITSINMUZAR] = binaryString[j];
		}
		temp=temp->next;
	}
	return TRUE;
}


int main() {
	Label *mLabel=NULL;
	FILE *f;
	Command *commandList[256];
	char binaryCodes[256][11];
	int i;
	memset(binaryCodes,'\0',MAXMEMORY*(MAXBITSINMUZAR+1));
	for (i=0; i<256; i++)
		commandList[i]=NULL;
	f = fopen("testdata1.txt","r");

	if (! f) {
		printf("The file is null\n");
	}
	firstPass(f, &mLabel, commandList);
	parseCommands(commandList, binaryCodes, mLabel);
	parseLabels(binaryCodes, mLabel);
	printLabels(&mLabel);
	for (i=0; i<38; i++)
		printf("%d : %s\n",i+100, binaryCodes[i]);
	return 0;
}
