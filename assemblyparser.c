#include <stdio.h>
#include "globaldefs.h"
#include "fileparserutils.h"
#include "datastructures.h"
#include "stringparserutils.h"
#include "conversions.h"
#include <string.h>
#include <stdlib.h>
#include "./assemblyparser.h"



/*this function goes over the algorithm from maman 14
its a bit long but was not simple to break down*/
int firstPass(FILE *aFile , Label **labelsList, Command *commandList[]) { 

	/*internal function variables*/	
	LabelType mDirective;
	char line[MAXSTRLEN];
	char tempLine[MAXSTRLEN];
	char *ptrToken;
	char *firstToken;
	char mLabelName[MAXSTRLEN];
	char mCleanLabelName[MAXSTRLEN];
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

		lineNumber++;
		if (isComment(line))
			continue;
		if(isEmpty(line))
			continue;
		strcpy(tempLine,line);
		ptrToken = strtok(tempLine," \t\n");  /*the first token*/
		firstToken = ptrToken;
		/*clean vars that need to be reused*/
		mDirective=errorlabel;
		memset(operandsToParse,'\0',MAXSTRLEN);
		memset(tempString,'\0',MAXSTRLEN);
		/*stage 3 - is the first item a label?*/
		if (isLabel(ptrToken)) {
			labelFlag=TRUE;
			strcpy(mLabelName, ptrToken);

			/*stage 4 - set the label flag and move to the next token*/
			labelFlag = TRUE;
			ptrToken = strtok(NULL," \t\n");
		}
		/*stage 5 - check if this is a directive*/
		mDirective = getDirective(ptrToken);
		if (mDirective) {
			if (mDirective==data || mDirective==string || mDirective==structlabel) {
				/*stage 6 - if there is a label, add it to the list*/
				if (labelFlag) {
					/*update error flag according to result of PushLabel*/
					cleanLabel(mCleanLabelName, mLabelName);
					errorFlag = ! safePushLabel(labelsList, mCleanLabelName, DC, mDirective);
				}
			/*stage 7 - calculate data length and update update DC*/
				ptrToken = strtok(NULL,"");
				strcpy(operandsToParse,ptrToken);
				if (mDirective==data) {
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
					errorFlag = ! getStruct(operandsToParse, &analyzedStruct);
					updateLabelStruct(labelsList, &analyzedStruct, mCleanLabelName);
					DC+=analyzedStruct.length;
				}
				if (errorFlag) {
					printf("ERROR: Aborting current parsing at line %d.Illegal directive operands: %s\n",lineNumber,operandsToParse );
					break;
				} else {
					continue;
				}

			}
		}
		/*stage 8*/
		if (!strcmp(firstToken,".extern")) {
			/*stage 9 - map externals*/
			firstToken = strtok(NULL,"");
			cleanLabel(mCleanLabelName, firstToken);
			errorFlag = ! safePushLabel(labelsList, mCleanLabelName, 0, external);
			/*stage 10 - go back to the next line*/
			continue;
		}
		if (!strcmp(firstToken,".entry")) {
			continue;
		}
		/*stage 11*/
		if (labelFlag) {
			cleanLabel(mCleanLabelName, mLabelName);

			errorFlag = ! safePushLabel(labelsList, mCleanLabelName,IC,command);
		}
		/*stage 12+13*/
		/*analyze rest of data*/
		analyzedCommand=(Command*)calloc(1,sizeof(Command));
		if (labelFlag) {
			strcpy(tempLine,line);
			ptrToken = strtok(tempLine," \t\n");
			ptrToken = strtok(NULL,"");
			getCommand(ptrToken, analyzedCommand);
		} else {
			strcpy(tempLine,line);
			ptrToken = strtok(tempLine," \r\n");
			if (ptrToken)
				strcat(tempString,ptrToken);
			ptrToken = strtok(NULL,"\n");
			if (ptrToken) {
				strcat(tempString," ");
				strcat(tempString,ptrToken);
			}
			getCommand(tempString,analyzedCommand);
		}

		enrichCommand(analyzedCommand);
		analyzedCommand->IC=IC;
		analyzedCommand->codeLine=lineNumber;
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
	for (i=0; i<MAXMEMORY; i++) {
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
		}
		/*add the parsed binary data to the binary array representing the memory*/
		for (j=0; j<strlen(binaryString); j++) {
			binaryCodes[temp->address + j/MAXBITSINMUZAR][j % MAXBITSINMUZAR] = binaryString[j];
		}
		temp=temp->next;
	}
	return TRUE;
}

int writeExternals(Command *commandList[], Label *labels, FILE *externalFile) {
	int i;
	char muzarAddress[3];
	for (i=0; i<MAXMEMORY; i++) {
		if (commandList[i]) {
			addressToMuzar(muzarAddress, commandList[i]->IC + BASEADDRESS);
			if (commandList[i]->operandNum >= 1){
				if ((commandList[i]->Operand1.oprType==label) && (getTypeByName(&labels,commandList[i]->Operand1.label)==external))
					fprintf(externalFile,"%s %s\n",muzarAddress, commandList[i]->Operand1.label);
				if ((commandList[i]->Operand1.oprType==structtype) && (getTypeByName(&labels,commandList[i]->Operand1.label)==external))
					fprintf(externalFile,"%s %s\n",muzarAddress, commandList[i]->Operand1.label);	
			}
			if (commandList[i]->operandNum==2) {
				if ((commandList[i]->Operand2.oprType==label) && (getTypeByName(&labels,commandList[i]->Operand2.label)==external))
					fprintf(externalFile,"%s %s\n",muzarAddress, commandList[i]->Operand2.label);
				if ((commandList[i]->Operand2.oprType==structtype) && (getTypeByName(&labels,commandList[i]->Operand2.label)==external))
					fprintf(externalFile,"%s %s\n",muzarAddress, commandList[i]->Operand2.label);
			}
		}
	}
	return TRUE;
} 

int getEntries (FILE *inputFile, FILE *entryFile, Label **head) {
	char *ptrToken;
	char line[MAXSTRLEN];
	char cleanLabelName[MAXSTRLEN];
	char binAddress[3];
	int lineCounter=0;
	int address=0;
	while (fgets(line, MAXSTRLEN, inputFile)) {
		lineCounter++;
		ptrToken=strtok(line," \t\n");
		if (ptrToken==NULL)
			continue;
		if (!strcmp(ptrToken,".entry")) {
			ptrToken=strtok(NULL," \t\n");
			if (ptrToken==NULL) {
				printf("ERROR: Incorect entry label at line %d\n", lineCounter);
				continue;
			}
			cleanLabel(cleanLabelName, ptrToken);
			address = getAddressByName(head, cleanLabelName);
			if (address==-1) {
				printf("ERROR: Label not found for entry directive at line %d\n", lineCounter);
				continue;
			}
			addressToMuzar(binAddress, address+BASEADDRESS);
			fprintf(entryFile, "%s %s\n", cleanLabelName, binAddress);
		}
	}
	return TRUE;
}

int assemblyParser(char  *baseFileName) {
	int i;
	Label *mLabel=NULL;
	Command *commandList[MAXMEMORY];
	char binaryCodes[MAXMEMORY][MAXBITSINMUZAR+1];
	char muzarCommand[3];
	char muzarAddress[3];
	char parsedInputFileName[MAXSTRLEN];
	char parsedOutputFileName[MAXSTRLEN];
	char externalFileName[MAXSTRLEN];
	char entryFileName[MAXSTRLEN];
	FILE *inputFile;
	FILE *outputFile;
	FILE *externalFile;
	FILE *entryFile;
	/*printf("going to parse: %s\n", baseFileName);*/
	/*reset some memory to be sure no garbage*/
	memset(binaryCodes,'\0',MAXMEMORY*(MAXBITSINMUZAR+1));
	for (i=0; i<MAXMEMORY; i++)
		commandList[i]=NULL;

	/*check file exists*/
	strcpy(parsedInputFileName,baseFileName);
	strcat(parsedInputFileName,".as");
	inputFile = fopen(parsedInputFileName,"r");
	if (! inputFile) {
		printf("ERROR: Failed to open file: %s\n",baseFileName);
		return FALSE;
	}
	strcpy(parsedOutputFileName, baseFileName);
	strcpy(externalFileName, baseFileName);
	strcpy(entryFileName, baseFileName);
	strcat(parsedOutputFileName, ".ob");
	strcat(externalFileName,".ext");
	strcat(entryFileName,".ent");

	outputFile = fopen(parsedOutputFileName,"w");
	externalFile = fopen(externalFileName,"w");
	entryFile = fopen(entryFileName,"w");

	if ((! outputFile) || (! externalFile) || (! entryFile)){
		printf("FATAL: could not open output files for writing. Terminating assembler.\n");
	}

	/*parse*/
	firstPass(inputFile, &mLabel, commandList);
	parseCommands(commandList, binaryCodes, mLabel);
	parseLabels(binaryCodes, mLabel);
	writeExternals(commandList, mLabel, externalFile);
	i=0;
	while(strlen(binaryCodes[i])==MAXBITSINMUZAR) {
		binCommandToMuzar(binaryCodes[i], muzarCommand);
		addressToMuzar(muzarAddress,i+BASEADDRESS);
		fprintf(outputFile,"%s %s\n",muzarAddress,muzarCommand);
		i++;
	}
	rewind(inputFile);
	getEntries(inputFile, entryFile, &mLabel);

	for (i=0; i<MAXMEMORY; i++) 
		free(commandList[i]);

	freeLabels(&mLabel);
	fclose(outputFile);
	fclose(inputFile);
	fclose(externalFile);
	fclose(entryFile);
	printf("Finished parsing %s\n",parsedInputFileName);
	return TRUE;
}
