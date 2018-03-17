#define BITSINMUZAR 5
#define DIGITSINCOMMAND 2


typedef char BinaryDigit[BITSINMUZAR];

typedef char BinaryString[BITSINMUZAR*DIGITSINCOMMAND];

int mPow(int base, int exp);
char int2Base32(int int2convert);
int bin2Base32Digit(BinaryDigit bin2convert);
int intToBinPos(char *returnStr, int intValue, int numOfBits);
int intToBin(char *returnStr, int intValue, int numOfBits);
int binCommandToMuzar(char *binCommand, char*muzarCommand);
int addressToMuzar(char *returnStr, int address);
