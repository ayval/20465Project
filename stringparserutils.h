int isNumber(char *stringToParse);
int isComment(char *stringToParse);
int isLabel(char *stringToParse);
int getNextToken(char *token, char *line);
int cleanString(char *cleanStr, char *strToClean);
int splitStruct(char *structString, char *firstPart, char *secondPart);
int isEmpty(char *stingToParse);
int cleanFromSpaces(char *cleanStr, char *strToClean);