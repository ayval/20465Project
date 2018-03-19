#include "./globaldefs.h"
#include "./stringparserutils.h"
#include "./conversions.h"
#include "./fileparserutils.h"


#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>


LabelType getDirective(char *stringToParse) {
	if (! stringToParse)
		return FALSE;
	if (! strcmp(stringToParse,".data"))
		return data;

	if (! strcmp(stringToParse,".string"))
		return string;

	if (! strcmp(stringToParse,".struct"))
		return structlabel;

	if (! strcmp(stringToParse,".entry"))
		return entry;
	
	if (! strcmp(stringToParse,".extern"))
		return external;
	return FALSE;
}


Operation getOperationFromToken(char *stringToParse) {
	if (! strcmp(stringToParse,"mov"))
		return mov;
	if (! strcmp(stringToParse,"cmp"))
		return cmp;
	if (! strcmp(stringToParse,"add"))
		return add;
	if (! strcmp(stringToParse,"sub"))
		return sub;
	if (! strcmp(stringToParse,"not"))
		return not;
	if (! strcmp(stringToParse,"clr"))
		return clr;
	if (! strcmp(stringToParse,"lea"))
		return lea;
	if (! strcmp(stringToParse,"inc"))
		return inc;
	if (! strcmp(stringToParse,"dec"))
		return dec;
	if (! strcmp(stringToParse,"jmp"))
		return jmp;
	if (! strcmp(stringToParse,"bne"))
		return bne;
	if (! strcmp(stringToParse,"red"))
		return red;
	if (! strcmp(stringToParse,"prn"))
		return prn;
	if (! strcmp(stringToParse,"jsr"))
		return jsr;
	if (! strcmp(stringToParse,"rts"))
		return rts;
	if (! strcmp(stringToParse,"stop"))
		return stop;
	return error;
}

Reg getRegisterFromToken(char *stringToParse) {
	if (! strcmp(stringToParse, "r1"))
		return r1;
	if (! strcmp(stringToParse, "r2"))
		return r2;
	if (! strcmp(stringToParse, "r3"))
		return r3;
	if (! strcmp(stringToParse, "r4"))
		return r4;
	if (! strcmp(stringToParse, "r5"))
		return r5;
	if (! strcmp(stringToParse, "r6"))
		return r6;
	if (! strcmp(stringToParse, "r7"))
		return r7;
	return regerror;
}


/*parses a string with a format of a command and places the raw data and operator in a command struct*/
int getCommand(char *stringToParse, Command *command) {
	char *ptrToken;
	int opToCheck;
	if (! command)
		return FALSE;
	if (! stringToParse)
		return FALSE;
	clearCommand(command);
	ptrToken = strtok(stringToParse," ,\t\n");
	opToCheck = getOperationFromToken(ptrToken);
	if (opToCheck==error)
		return FALSE;
	command->op = opToCheck;
	command->operandNum=0;
	ptrToken = strtok(NULL," ,\t\n");
	if (ptrToken!=NULL) {
		strcpy(command->Operand1.rawData,ptrToken);
		command->operandNum=1;
	} else 
		return TRUE;
	ptrToken = strtok(NULL," ,\t\n");
	if (ptrToken!=NULL) {
		strcpy(command->Operand2.rawData,ptrToken);
		command->operandNum=2;
	}
	return TRUE;

}

/*adds data to the operand struct accordibng to the rawdata*/
int enrichOperand(Operand *operand) {
	char tempStr[MAXOPERANDLENGTH];
	if (! operand)
		return FALSE;
	/*check if the operand is a register and return which*/
	operand->regValue = getRegisterFromToken(operand->rawData);
	if (operand->regValue!=regerror) {
		operand->oprType=reg;
		return TRUE;
	}
	if (operand->rawData[0]=='#' && isNumber(operand->rawData+1)) {
		operand->constValue=atof(operand->rawData+1);
		operand->oprType=number;
		return TRUE;
	}
	operand->structFieldType = isStructField(operand->rawData,operand->label);
	if (operand->structFieldType) {
		operand->oprType = structtype;
		return TRUE;
	}
	strcpy(tempStr,operand->rawData);
	if(isAddress(tempStr)) {
		operand->oprType = label;
		return TRUE;
	}
	return FALSE;
}

int clearCommand(Command *command) {
	if (!command)
		return FALSE;
	command->op=error;
	command->operandNum=0;
	command->Operand1.oprType=operror;
	command->Operand2.oprType=operror;
	command->Operand1.constValue=0;
	command->Operand2.constValue=0;
	command->Operand1.structFieldType=0;
	command->Operand2.structFieldType=0;
	command->Operand1.regValue=0;
	command->Operand2.regValue=0;
	command->codeLine=0;
	memset(command->Operand1.rawData,'\0',MAXOPERANDLENGTH);
	memset(command->Operand2.rawData,'\0',MAXOPERANDLENGTH);
	memset(command->Operand1.label,'\0',MAXOPERANDLENGTH);
	memset(command->Operand2.label,'\0',MAXOPERANDLENGTH);
	command->IC=0;
	command->type=operror;
    command->operandNum=0;
    return TRUE;
}

int enrichCommand(Command *command) {
	/*enrich the operands and sanity check those are not null pointers*/
	if (! enrichOperand(&command->Operand1))
		return FALSE;
	if (! enrichOperand(&command->Operand2))
		return FALSE;
	return TRUE;
}

/*for debugging only - print the command in human readable way*/
void printCommand(Command *command) {
	printf("__________________\nOperation: %d   NumOfOperands=%d\n Operand1: '%s'    Operand2: '%s' \n", command->op
		, command->operandNum, command->Operand1.rawData, command->Operand2.rawData);
	if (command->Operand1.oprType == reg)
		printf(" Operand1: register:%d\n ", command->Operand1.regValue);
	if (command->Operand2.oprType ==reg)
		printf(" Operand2: register:%d\n ", command->Operand2.regValue);
	if (command->Operand1.oprType == number)
		printf(" Operand1: number:%f\n ", command->Operand1.constValue);
	if (command->Operand2.oprType == number)
		printf(" Operand2: number:%f\n ", command->Operand2.constValue);
	if (command->Operand1.oprType == structtype)
		printf(" Operand1: struct: label:%s field:%d\n", command->Operand1.label, command->Operand1.structFieldType);
	if (command->Operand2.oprType == structtype)
		printf(" Operand2: struct: label:%s field:%d\n", command->Operand2.label, command->Operand2.structFieldType);
}


/*returns 2 for struct and 1 for all other operand types*/
int getAdditionalWordCountForOperand(Operand *operand) {
	if (operand->oprType==structtype)
		return 2;
	return 1;
}

int getAdditionalWordCount(Command *command) {
	/* zero operands always returns 0*/
	if (requiredOperandNum(command)==0)
		return 0;
	/* one operand returns 0 if reg and 1 in other cases*/
	if (requiredOperandNum(command)==1) {
		if (command->Operand1.oprType == reg)
			return 0;
		return (getAdditionalWordCountForOperand(&command->Operand1));
	}
	/*assuming 2 operands*/
	/*first case is unique - 2 registers*/
	if (command->Operand1.oprType==reg && command->Operand2.oprType==reg)
			return 1;
	/*general case for 2 operands*/
	return getAdditionalWordCountForOperand(&command->Operand1) + getAdditionalWordCountForOperand(&command->Operand2);
}

int requiredOperandNum(Command *command) {
	switch(command->op) {
		case mov:
		case cmp:
		case add:
		case sub:
		case lea:
			return 2;
		case not:
		case clr:
		case inc:
		case dec:
		case jmp:
		case bne:
		case red:
		case prn:
		case jsr:
			return 1;
		case stop:
		case rts:
			return 0;
		default:
			return error;
	}
	return error;
}

/*builds a data struct from the data list operands list. Returns  FALSE if not valid*/
int getData(AData *dataToReturn, char *dataToParse) {
	char *token;
	char s[2]=",";
	int counter=0;
	/*safe check*/
	if (!dataToParse || dataToParse[0]=='\0')
		return FALSE;
	/*break the data to different tokens*/
	token = strtok(dataToParse,s);
	/*firts token*/
	if (isNumber(token)) {
		dataToReturn->data[counter]=atoi(token);
		counter++;
	} else {
		return FALSE;
	}
	/*rest of the tokens*/
	while (token != NULL) {
		token = strtok(NULL, s);

		if (isNumber(token)) {
			dataToReturn->data[counter]=atoi(token);
			counter++;
		} else {
			if (! (token=='\0')) {
				return FALSE;
			}
		}
	}
	dataToReturn->length=counter;
	return TRUE;
}

/*converts an array from an AData struct to a binary muzar representation*/
int getBinData(char *stringToReturn, AData *dataToParse) {
	int i;
	char tempStr[MAXBITSINMUZAR+1];
	if ((! stringToReturn) || (! dataToParse))
		return FALSE;
	for (i=0; i<dataToParse->length; i++) {
		intToBin(tempStr,dataToParse->data[i],MAXBITSINMUZAR);
		strcat(stringToReturn, tempStr);
	}
	return TRUE;
}


/*converts a string to a binary muzar representation*/
int getBinString(char *stringToReturn, char *dataToParse) {
	int i;
		char tempStr[MAXBITSINMUZAR+1];
	if ((! stringToReturn)|| (! dataToParse))
		return FALSE;
	for (i=0; i<strlen(dataToParse); i++) {
		intToBin(tempStr, dataToParse[i], MAXBITSINMUZAR);
		strcat(stringToReturn, tempStr);
	} 
	/*add the string terminator*/
	strcat(stringToReturn, BINARYSTRINGTERMINATOR);
	return TRUE;
}

/* converts a struct to binary muzar representation*/
int getBinStruct(char *stringToReturn, AStruct *structToParse) {
	char tempStr[MAXBITSINMUZAR*MAXMEMORY];
	memset(tempStr,'\0', MAXBITSINMUZAR*MAXMEMORY);
	if ((! stringToReturn)|| (! structToParse))
		return FALSE;
	if (! getBinString(tempStr, structToParse->data))
		return FALSE;
	intToBin(stringToReturn, structToParse->number, MAXBITSINMUZAR);
	strcat(stringToReturn, tempStr);
	return TRUE;
}



int getStruct(char *structToParse, AStruct *structToReturn) {
	int error;
	char numStr[MAXSTRLEN];
	char strStr[MAXSTRLEN];
	error = splitStructLabel(structToParse, numStr, strStr);
	if (! error)
		return FALSE;
	error = cleanString(structToReturn->data, strStr);
	if (! error)
		return FALSE;
	if (! isNumber(numStr))
		return FALSE;
	structToReturn->number=atoi(numStr);
	/*the struct data length is the string + 1 for terminator and 1 for the number*/
	structToReturn->length = strlen(structToReturn->data) +2;
	return TRUE;
}

int getString(char *stringToParse, AStruct structToReturn) {
	return cleanString(structToReturn.data, stringToParse);
}

int commandToBin(char *MuzarStr, Command *command, Label *labels) {
	char cmdBinStr[MAXBITSINMUZAR]="";
	char cmdAddStr[MAXBITSINMUZAR]="";
	char tempStr[20];
	int addressing=0;
	/*code the operation to bin*/
	intToBin(cmdBinStr,command->op,4);
	strcat(MuzarStr,cmdBinStr);
	/*code the addressing types*/
	if (command->operandNum>=1) {
		if (command->Operand1.oprType==reg)
			addressing+=3;
		if (command->Operand1.oprType==structtype)
			addressing+=2;
		if (command->Operand1.oprType==label)
			addressing+=1;
	}
	if (command->operandNum==2) {
			addressing*=4;
		if (command->Operand2.oprType==reg)
			addressing+=3;
		if (command->Operand2.oprType==structtype)
			addressing+=2;
		if (command->Operand2.oprType==label)
			addressing+=1;
	}
	intToBin(cmdAddStr,addressing,4);
	strcat(MuzarStr,cmdAddStr);
	/*always direct for commands*/
	strcat(MuzarStr,"00");
	/*no operands*/
	if (command->operandNum==0) {
		return TRUE;
	}
	/*special case: two operands. Both are registers*/
	if ((command->operandNum==2) && (command->Operand1.oprType==reg) && (command->Operand2.oprType==reg)){
		memset(tempStr,'\0',20);
		intToBin(tempStr,command->Operand1.regValue,4);
		strcat(MuzarStr,tempStr);
		intToBin(tempStr,command->Operand2.regValue,4);
		strcat(MuzarStr,tempStr);
		strcat(MuzarStr,"00");
		return TRUE;
	}
	/*single operand*/
	if (command->operandNum>=1) {
		memset(tempStr,'\0',20);
		if (! operandToBin(tempStr, &command->Operand1, labels)) {
			printf("ERROR: Line %d: Command of type %d parsing failed. First operand failed parsing.\n", command->codeLine, command->op);
			return FALSE;
		}
		strcat(MuzarStr,tempStr);
		/*return only if completed evaluating operands*/
		if (command->operandNum==1)
			return TRUE;
	}
	if (command->operandNum==2) {
		memset(tempStr,'\0',20);
		if (! operandToBin(tempStr, &command->Operand2, labels)) {
			printf("ERROR: Line %d: Command of type %d parsing failed. Second operand failed parsing\n", command->codeLine, command->op);
			return FALSE;
		}
		strcat(MuzarStr,tempStr);
		return TRUE;
	}
	printf("ERROR: Line %d: Unknown error when evaluating command of type %d. Num of operands received %d\n", command->codeLine, command->op, command->operandNum);
	return FALSE;
}


int operandToBin(char *returnStr, Operand *operand, Label *labels) {
	char tempStr[MAXBITSINMUZAR+1];
	int labelAddress=0;
	if (operand->oprType==reg) {
		intToBin(returnStr, operand->regValue, 4);
		/*0000 padding and ARE=00*/
		strcat(returnStr, "000000");
		return TRUE;
	}
	if (operand->oprType==number) {
		intToBin(returnStr, operand->constValue, 8);
		/*ARE 00 for number*/
		strcat(returnStr, "00");
		return TRUE;
	}
	if (operand->oprType==label) {
		labelAddress=getAddressByName(&labels, operand->label);
		if (labelAddress==ADDRESSNOTFOUND) {
			printf("ERROR: Operand parsing failed. String address %s does not exist\n",operand->label);
			return FALSE;
		}
		intToBin(returnStr, labelAddress+BASEADDRESS,8);
		strcat(returnStr, "10");
		return TRUE;
	}
	if (operand->oprType==structtype) {
		labelAddress=getAddressByName(&labels, operand->label);
		if (labelAddress==ADDRESSNOTFOUND) {
			printf("ERROR: Operand parsing failed. Struct address %s does not exist\n",operand->label);
			return FALSE;
		}
		intToBin(returnStr, labelAddress+100,8);
		strcat(returnStr, "10");
		intToBin(tempStr, operand->structFieldType,8);
		strcat(returnStr,tempStr);
		strcat(returnStr,"00");
		return TRUE;
	}
	printf("ERROR: Operand type not recognized.\n");
	return FALSE;
}
