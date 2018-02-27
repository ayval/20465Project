#define MAXOPERANDLENGTH 200
typedef enum directive {
        NotDirective=0,
        DataDirective=1,
        StringDirective=2,
        StructDirective=3,
        EntryDirective=4,
	ExternalDirective=5
} Directive;

typedef enum operation {
        error=-1,
        mov=0,
        cmp=1,
        add=2,
        sub=3,
        not=4,
        clr=5,
        lea=6,
        inc=7,
        dec=8,
        jmp=9,
        bne=10,
        red=11,
        prn=12,
        jsr=13,
        rts=14,
        stop=15
} Operation;

typedef struct command {
	Operation op;
	char Operand1[MAXOPERANDLENGTH];
	char Operand2[MAXOPERANDLENGTH];
	int type;
} Command;

Directive getDirective(char *stringToParse);
Operation getOperationFromToken(char *stringToParse);
int isNumber(char *stringToParse);
int isComment(char *stringToParse);
int isLabel(char *stringToParse);
int getNextToken(char *token, char *line);
