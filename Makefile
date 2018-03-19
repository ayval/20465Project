CC=gcc
CFLAGS=-Wall -pedantic -ansi

assembler: assembler.o assemblyparser.o fileparserutils.o datastructures.o stringparserutils.o conversions.o
	$(CC) -o assembler assembler.o assemblyparser.o fileparserutils.o datastructures.o stringparserutils.o conversions.o

