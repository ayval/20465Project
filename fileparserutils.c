#include "./fileparserutils.h"
#include "./stringparserutils.h"
#include "./globaldefs.h"
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
	if (! operand)
		return FALSE;
	/*check if the operand is a register and return which*/
	operand->regValue = getRegisterFromToken(operand->rawData);
	if (operand->regValue!=regerror) {
		operand->oprType=reg;
	}
	return TRUE;
}

int clearCommand(Command *command) {
	if (!command)
		return FALSE;
	command->op=error;
	command->Operand1.oprType=operror;
	command->Operand2.oprType=operror;
	command->Operand1.intValue=0;
	command->Operand2.intValue=0;
	memset(command->Operand1.rawData,'\0',MAXOPERANDLENGTH);
	memset(command->Operand2.rawData,'\0',MAXOPERANDLENGTH);
	command->type=operror;
    command->operandNum=0;
    return TRUE;
}

int enrichCommand(Command *command) {
	/*enrich the operands and sanity check those are not null pointers*/
	if (! enrichOperand(&command->Operand1))
		return FALSE;
	if (! enrichOperand(&command->Operand1))
		return FALSE;
	return TRUE;
}

/*for debugging only - print the command in human readable way*/
void printCommand(Command *command) {
	printf("Operation: %d   NumOfOperands=%d, Operand1: '%s'    Operand2: '%s' \n", command->op
		, command->operandNum, command->Operand1.rawData, command->Operand2.rawData);
	if (command->Operand1.oprType == reg)
		printf("Operand1: register:%d",command->Operand1.regValue);
	if (command->Operand2.oprType ==reg)
		printf("Operand2: register:%d",command->Operand2.regValue);


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
	error = splitStruct(structToParse, numStr, strStr);
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

