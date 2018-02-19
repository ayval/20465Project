#include "./fileparserutils.h"
#include "./globaldefs.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>




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

int isNumber(char *stringToParse) {
	int i=0;
	/*check null*/
	if (! stringToParse)
		return FALSE;
	/*check for sign*/
	if ((stringToParse[0]=='+') || (stringToParse[0]=='-')) {
		/*second char must be digit if the first was a sign*/
		if (! isdigit(stringToParse[1]))
			return FALSE;
		i++; /*start checking only from the second if there is a sign*/
	}
	/*check the rest of the chars for digits*/
	while(stringToParse[i]) {
		if (! isdigit(stringToParse[i]))
			return FALSE;
		i++;
	}
	return TRUE;
}


Directive getDirective(char *stringToParse) {
	if (! strcmp(stringToParse,".data"))
		return DataDirective;

	if (! strcmp(stringToParse,".string"))
		return StringDirective;

	if (! strcmp(stringToParse,".data"))
		return StructDirective;

	if (! strcmp(stringToParse,".data"))
		return EntryDirective;
	return FALSE;
}

int isLabel(char *stringToParse) {
	int i=0;
	/*protect againt empty string*/
	if (!stringToParse[0])
		return FALSE;
	/*find first non uppercase char*/
	do {	
		if(! isupper(stringToParse[i])) {
			break;
			}
		i++;
	} while(stringToParse[i]);

	/*if the first non uppercase char is ':' and it is also the last in the string, this is a lable*/
	if (stringToParse[i]==':' && i+1==strlen(stringToParse))
		return TRUE;
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


/*
int main(){
	Operation op;
	char teststr[] = "mov";
	if (isLabel(teststr))
		printf("This is a label\n");
	else 
		printf("This is not a label\n");

	op = getOperationFromToken("prn");	
	if (op)
			printf("this is an op: %d\n",op);
	else
		printf("This is not an op\n");

	if (isNumber("123"))
		printf("123 is a number\n");
	if (isNumber("+123"))
		printf("+123 is a number\n");
	if(isNumber("-123"))
		printf("-123 is a number\n");
	if(isNumber("33b"))
		printf("33b is a number\n");
	if(isNumber("3+3"))
		printf("3+3 is a number\n");

	return 1;
}
*/

