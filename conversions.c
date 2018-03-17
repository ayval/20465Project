#include "./globaldefs.h"
#include <string.h>
#include<stdio.h>
#include "./conversions.h"


/*simple power for integers, only for positive. Not safe.*/
int mPow(int base, int exp) {
	int result=1;
	while(exp) {
		result*=base;
		exp--;
	}
	return result;
}

char int2Base32(int int2convert) {
	/*shortcut and nicer code for 11-31*/
	if (int2convert>=10 && int2convert<32)
		return int2convert-10+'a';

	switch (int2convert) {
		/*the more unique cases that are not covered well by range --> 0-10*/
		case 0:
			return '!';
			break;
		case 1:
			return '@';
			break;
		case 2:
			return '#';
			break;
		case 3:
			return '$';
			break;
		case 4:
			return '%';
			break;
		case 5:
			return '^';
			break;
		case 6:
			return '&';
			break;
		case 7:
			return '*';
			break;
		case 8:
			return '<';
			break;
		case 9:
			return '>';
			break;
		default:
			return -1;
		}
}

int bin2Base32Digit(BinaryDigit bin2convert) {
	int result=0;
	int i;
	for (i=0; i<BITSINMUZAR; i++) {
		if (bin2convert[i]=='1') {
			result+=mPow(2,BITSINMUZAR-1-i);
		}
	}
	return int2Base32(result);

}

/*return the binary string equivalent of a positive integer*/
int intToBinPos(char *returnStr, int intValue, int numOfBits) {
	int i;
	/*safety check that a positive value was given*/
	if (intValue<0)
		return FALSE;
	memset(returnStr,'0',numOfBits);

	returnStr[numOfBits]='\0';
	for (i=numOfBits-1;i>=
		0;i--) {
		if (intValue % 2)
			returnStr[i]='1';
		else
			returnStr[i]='0';
		intValue /=2;
	}
	returnStr[numOfBits]='\0';
	return TRUE;
}


int intToBin(char *returnStr, int intValue, int numOfBits) {
	if (intValue>=0)
		return intToBinPos(returnStr, intValue, numOfBits);
	return intToBinPos(returnStr, mPow(2,numOfBits)+intValue,numOfBits);
}


/*
int main() {
	char testData[20];
	intToBin(testData,-5,8);
	printf("-5=%s\n",testData);	
	printf("2**8 is: %d\n",mPow(2,8));
}

*/



