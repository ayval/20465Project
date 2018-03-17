#ifndef GLOBALDEFS
#define GLOBALDEFS

#define TRUE 1
#define FALSE 0
#define ADDRESSNOTFOUND -1
#define COMMENTCHAR ';'
#define MAXSTRLEN 250
#define MAXMEMORY 256
#define MAXBITSINMUZAR 10
#define MAXOPERANDLENGTH 200
#define BINARYSTRINGTERMINATOR "0000000000"
#define BASEADDRESS 100

typedef enum labelType {
		errorlabel=0,
        data=1,
        string=2,
        structlabel=3,
        external=4,
        entry=5,
        command=6
} LabelType;

#endif




