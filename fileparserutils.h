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

typedef enum reg {
        r0=0,
        r1=1,
        r2=2,
        r3=3,
        r4=4,
        r5=5,
        r6=6,
        r7=7,
        regerror=8
} Reg;

typedef enum operandType {
        reg=1,
        label=2,
        structtype=3,
        number=4,
        operror=0
} OperandType;

typedef enum structField {
        integerField=1,
        stringField=2,
        errorField=0
} StructField;

typedef struct operand {
        char rawData[MAXOPERANDLENGTH];
        OperandType oprType;
        float constValue;
        Reg regValue;
        char label[MAXOPERANDLENGTH];
        StructField structFieldType;
} Operand;

typedef struct command {
	Operation op;
	Operand Operand1;
	Operand Operand2;
	int type;
        int operandNum;
        int IC;
} Command;

typedef struct aStruct {
	char data[MAXOPERANDLENGTH];
	int number;
	int length;
} AStruct;

typedef struct aData {
	int data[80];
	int length;
} AData;


Directive getDirective(char *stringToParse);
Operation getOperationFromToken(char *stringToParse);
int getData(AData *dataToReturn, char *dataToParse);
int getStruct(char *stringToParse, AStruct structToReturn);
int getCommand(char *string, Command *command);
void printCommand(Command *command);
int enrichCommand(Command *command);
int enrichOperand(Operand *operand);
int clearCommand(Command *command);
int requiredOperandNum(Command *command);
int getAdditionalWordCount(Command *command);
int commandToBin(char *MuzarStr, Command *command);
int operandToBin(char *returnStr, Operand *operand);
