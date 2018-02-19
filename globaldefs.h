#define TRUE 1
#define FALSE 0
#define BITSINMUZAR 5
#define DIGITSINCOMMAND 2
#define COMMENTCHAR ';'
#define MAXSTRLEN 250

const char WHITESPACE[2] = " ";

typedef char BinaryDigit[BITSINMUZAR];

typedef char BinaryString[BITSINMUZAR*DIGITSINCOMMAND];

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


typedef enum directive {
	NotDirective=0,
	DataDirective=1,
	StringDirective=2,
	StructDirective=3,
	EntryDirective=4
} Directive;
