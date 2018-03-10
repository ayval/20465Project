#include "./stringparserutils.h"
#include<ctype.h>
#include<string.h>
#include "globaldefs.h"
#include<stdio.h> /*remove before submitting*/



int isLabel(char *stringToParse) {
	int i=0;
	/*protect againt empty string*/
	if (!stringToParse[0])
		return FALSE;
	/*first character must be alphanumberic*/
	if (!isalpha(stringToParse[0]))
		return FALSE;
	/*find first non uppercase char*/
	do {	
		if(! isalnum(stringToParse[i])) {
			break;
			}
		i++;
	} while(stringToParse[i]);

	/*if the first non uppercase char is ':' and it is also the last in the string, this is a lable*/
	if (stringToParse[i]==':' && i+1==strlen(stringToParse))
		return TRUE;
	return FALSE;
}

int isComment(char *stringToParse) {
	/*check null*/
	if (! stringToParse)
		return FALSE;
	/*check first char is #*/
	if (stringToParse[0]==COMMENTCHAR) {
		return TRUE;
	}
	/*not a comment*/
	return FALSE;
}

int isEmpty(char *stringToParse) {
	/*sanity check for null*/
	int i=0;
	if (! stringToParse)
		return FALSE;
	while (isspace(stringToParse[i]))
		i++;
	if (stringToParse[i]!='\0')
		return TRUE;
	return FALSE;
}


int isNumber(char *stringToParse) {
	int i=0;
	/*check null*/
	if (! stringToParse)
		return FALSE;
	/*ignore leading whitespaces*/
	while (isspace(stringToParse[i]))
		i++;
	/*check for sign*/
	if ((stringToParse[i]=='+') || (stringToParse[i]=='-')) {
		/*second char must be digit if the first was a sign, safe since at the worst case its \0*/
		if (! isdigit(stringToParse[i+1])){
			return FALSE;
		}
		i++; /*start checking only from the second if there is a sign*/
	}
	/*check the rest of the chars for digits*/
	while(stringToParse[i]) {
		if (isdigit(stringToParse[i]))
			i++;
		else break;
	}
	if (! stringToParse[i])
		return TRUE;
	/*ignore trailing whitespaces*/
	while(stringToParse[i])
		if(isspace(stringToParse[i]))
			i++;
		else
			return FALSE;
	return TRUE;
}

int cleanString(char *cleanStr, char *strToClean) {
	int i=0;
	int j=0;
	/*sanity check for null pointer*/
	if (! strToClean)
		return FALSE;
	/*ignore leading white spaces*/
	while(isspace(strToClean[i]))
		i++;
	/*first char should be "*/
	if (strToClean[i]!='"')
		return FALSE;
	i++;
	while (strToClean[i]!='"') {
		/*string terminator before closing string*/
		if (strToClean=='\0')
			return FALSE;
		cleanStr[j]=strToClean[i];
		i++;
		j++;
	}
	cleanStr[j]='\0';
	/*add code to check if any remaining string*/
	return TRUE;
}

int cleanFromSpaces(char *cleanStr, char *strToClean) { 
	int i=0;
	int j=0;
	/*sanity check for null pointer*/
	if (! strToClean)
		return FALSE;
	/*ignore leading white spaces*/
	while(!strToClean[i]=='\0' && isspace(strToClean[i])) {
		i++;
	}

	if (strToClean[i]=='\0')
		return FALSE;
	while(strToClean!='\0' && !isspace(strToClean[i])) {
		cleanStr[j]=strToClean[i];
		i++;
		j++;
	}
	cleanStr[j]='\0';
	return TRUE;
}


int getNextToken(char *token, char *line) {
	int tokenIndex=0;
	int lineIndex=0;
	while(isspace(line[lineIndex]))
		lineIndex++;
	while(!line[lineIndex]=='\0' && !isspace(line[lineIndex])) {
		token[tokenIndex]=line[lineIndex];
		tokenIndex++;
		lineIndex++;
	}
	token[tokenIndex]='\0';
	return lineIndex;
}



/*Just splits a string to two parts, before and after comma. If no comma, returns false*/
int splitString(char *structString, char *firstPart, char *secondPart, char delimiter) {
	int i=0;
	int j=0;
	/*copy the fits part*/
	while (structString[i]!=',' && structString[i]!='\0'){
		firstPart[i]=structString[i];
		i++;
	}
	/*sanity check that there are 2 parts*/
	if (structString[i]=='\0')
		return FALSE;
	/*add the null on account of the comma*/
	firstPart[i]='\0';
	i++;
	/*copy the second part*/
	while(structString[i]!='\0') {
		secondPart[j]=structString[i];
		i++;
		j++;
	}
	return TRUE;
}

int splitStructLabel(char *structString, char *firstPart, char *secondPart) {
	return splitString(structString, firstPart, secondPart, ',');
}

/* returns the field number. 1 for the string, 2 for the integer, 0 is an error occursed. The structName as a reference
assumes the stringToParse has been cleaend with no extra spaces*/
int isStructField(char *stringToParse, char *structName) {
	int i=0;
	if (! stringToParse)
		return FALSE;
	while (stringToParse[i]!='.' && stringToParse[i]!='\0') {
		structName[i]=stringToParse[i];
		i++;
	}
	structName[i]='\0';
	if (stringToParse[i]=='.') {
		if (stringToParse[i+1]=='1' && stringToParse[i+2]=='\0')
			return 1;
		if (stringToParse[i+1]=='2' && stringToParse[i+2]=='\0')
			return 2;
	}
	return FALSE;
}


