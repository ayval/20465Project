typedef char RegisterString[4]; 


int isNumber(char *stringToParse);
int isComment(char *stringToParse);
int isLabel(char *stringToParse);
int isStructField(char *stringToParse, char *structName);
int getNextToken(char *token, char *line);
int cleanString(char *cleanStr, char *strToClean);
int splitStructLabel(char *structString, char *firstPart, char *secondPart);
int isEmpty(char *stingToParse);
int cleanFromSpaces(char *cleanStr, char *strToClean);
int isAddress(char *stringToParse);
int cleanLabel(char *cleanLabel, char *labelToClean);
int trimwhitespace(char *str);
