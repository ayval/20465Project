#include "./fileparserutils.h"
#include "./stringparserutils.h"
#include "./globaldefs.h"
#include "./conversions.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>


Directive getDirective(char *stringToParse) {
	if (! strcmp(stringToParse,".data"))
		return DataDirective;

	if (! strcmp(stringToParse,".string"))
		return StringDirective;

	if (! strcmp(stringToParse,".struct"))
		return StructDirective;

	if (! strcmp(stringToParse,".entry"))
		return EntryDirective;
	
	if (! strcmp(stringToParse,".extern"))
		return ExternalDirective;
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

/*get the operator and operands as strings and places them in the command struct*/
int getCommand(char *stringToParse, Command *command) {
	char checkStr[MAXSTRLEN];
	Operation opToCheck;
	int i=0;
	int j=0;
	char tempStr[MAXSTRLEN];
	/*sanity check*/
	if (! stringToParse || ! command)
		return FALSE;
	clearCommand(command);
	/*ignore leading white spaces*/
	while (isspace(stringToParse[i]))
		i++;
	/*copy the first word as the command and leave the rest as operands*/
	while (stringToParse[i] && ! isspace(stringToParse[i])) {
		checkStr[j]=stringToParse[i];
		i++;
		j++;
	}
	checkStr[j]='\0';
	opToCheck = getOperationFromToken(checkStr);
	if (opToCheck==error)
		return FALSE;
	command->op = opToCheck;
	/*get the first operand*/
	j=0;
	while (stringToParse[i]!='\0' && stringToParse[i]!=',') {
		tempStr[j]=stringToParse[i];
		i++;
		j++;
	}
	cleanFromSpaces(command->Operand1.rawData,tempStr);
	if (stringToParse[i]=='\0') {
		command->operandNum=1;
		return TRUE;
	}

	/*get the second operand*/
	j=0;
	i++;
	memset(tempStr,'\0',MAXSTRLEN);
	while (stringToParse[i]!='\0' && stringToParse[i]!=',') {
		/*command->Operand2.rawData[j]*/

		tempStr[j]=stringToParse[i];
		i++;
		j++;
	}
	cleanFromSpaces(command->Operand2.rawData,tempStr);
	/*check for remaining data*/
	while(!stringToParse[i]=='\0' || isspace(stringToParse[i]))
		i++;
	if (!stringToParse[i]=='\0') {
		printf("RESIDUE!!!\n");
		return FALSE;
	}
	command->operandNum=2;
	return TRUE;
}

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
	printf("string to clean:%s\n",operand->rawData);
	if (operand->structFieldType) {
		operand->oprType = structtype;
		return TRUE;
	}
	strcpy(tempStr,operand->rawData);
	printf("Checking if is a label\n");
	if(isAddress(tempStr)) {
		printf("Its a label!\n");
		operand->oprType = label;
		return TRUE;
	}
	printf("its not a label!\n");

	return FALSE;
}

int clearCommand(Command *command) {
	if (!command)
		return FALSE;
	command->op=error;
	command->Operand1.oprType=operror;
	command->Operand2.oprType=operror;
	command->Operand1.constValue=0;
	command->Operand2.constValue=0;
	command->Operand1.structFieldType=0;
	command->Operand2.structFieldType=0;
	memset(command->Operand1.rawData,'\0',MAXOPERANDLENGTH);
	memset(command->Operand2.rawData,'\0',MAXOPERANDLENGTH);
	memset(command->Operand1.label,'\0',MAXOPERANDLENGTH);
	memset(command->Operand2.label,'\0',MAXOPERANDLENGTH);

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
	printf("Operation: %d   NumOfOperands=%d\n Operand1: '%s'    Operand2: '%s' \n", command->op
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

int validateCommandOperators(Command *command) {
	return TRUE;
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

/*builds a data struct from the data list operands list. Returns  FALSE if n0t valid*/
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
			if (! token=='\0') {
				return FALSE;
			}
		}
	}
	dataToReturn->length=counter;
	return TRUE;
}


int getStruct(char *structToParse, AStruct structToReturn) {
	/*ignore leading spaces*/
	int error;
	char numStr[MAXSTRLEN];
	char strStr[MAXSTRLEN];
	error = splitStructLabel(structToParse, numStr, strStr);
	if (! error) 
		return FALSE;
	error = cleanString(structToReturn.data, strStr);
	if (! error)
		return FALSE;
	if (! isNumber(numStr))
		return FALSE;
	structToReturn.number=atoi(numStr);
	/*the struct data length is the string + 1 for terminator and 1 for the number*/
	structToReturn.length = strlen(structToReturn.data) +2;
	return TRUE;
}

int getAREBits(Command *command) {
	return FALSE;

}


int commandToBin(char *MuzarStr, Command *command) {
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
	printf("addressing is: %d\n",addressing);
	intToBin(cmdAddStr,addressing,4);
	strcat(MuzarStr,cmdAddStr);
	strcat(MuzarStr,"00");
	if (command->operandNum==0)
		return TRUE;
	if (command->operandNum==1) 
		memset(tempStr,'\0',20);
		operandToBin(tempStr, &command->Operand1);
		strcat(MuzarStr,tempStr);
		return TRUE;
	return TRUE;
}


int operandToBin(char *returnStr, Operand *operand) {
	if (operand->oprType==reg) {
		intToBin(returnStr, operand->regValue, 4);
		/*0000 padding and ARE=00*/
		strcat(returnStr, "000000");
	}
	return TRUE;
}

