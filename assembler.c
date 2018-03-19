/* this is the main of the assembler program
Author: Ayval Ron (alone...) 
ID: 034539833
Completed on 19/3/2018
*/


#include "./assemblyparser.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
	int i;
	if (argc<2) {
		printf("No assembly files were found\n");
		return 0;
	}
	for (i=1; i<argc; i++)
		assemblyParser(argv[i]);
	return 0;
}
