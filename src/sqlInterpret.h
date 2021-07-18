#ifndef __SQLINTERPRET_H__ 
#define __SQLINTERPRET_H__

#include "utils.h"
typedef struct {
    char *fileName;
    char *key;
}member;

typedef struct {
    member *place;
    char *value;
}condition;

typedef struct {
    stringArray from;
    condition *where;
    member *select;
}command;

stringArray getInstructions();

command *separateCommands(stringArray instructionsArray);

char** getSourceFiles(char **instArray, int *numberOfFiles);

void commandGap(int *startIndex, int *size, char *begin, char *final, char **instArray);

void freePointers(command *instruction, stringArray instructionsArray);


#endif