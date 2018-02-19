#include <stdio.h>
#include "./globaldefs.h"
#include <math.h>


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
	char b32result;
	for (int i=0; i<BITSINMUZAR; i++) {
		if (bin2convert[i]=='1') {
			result+=pow(2,BITSINMUZAR-1-i);
		}
	}
	return int2Base32(result);

}


int main() {
	BinaryDigit bindig = "10101";
	for (int i=0; i<32; i++) {
		printf("%d:%c\n",i,int2Base32(i));
	}
	printf("10101=%c\n",bin2Base32Digit("00011"));	
}


