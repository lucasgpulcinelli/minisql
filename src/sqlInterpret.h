#ifndef __SQLINTERPRET_H__ 
#define __SQLINTERPRET_H__

#include "utils.h"
typedef struct {
    char *fileName;
    char *key;
}member;

typedef struct {
    member *place;
    char *comparationValue;
}condition;

typedef struct {
    stringArray from;
    condition *where;
    int whereSize;
    member *select;
    int selectSize;
}command;

stringArray getInstructions();

command *processInstructions(stringArray instructionsArray);

member *getSelection(stringArray instArray, int *amount);

condition *getConditions(stringArray instArray, int *amount);

char** getSourceFiles(stringArray instArray, int *numberOfFiles);

void isolateCommand(int *startIndex, int *size, char *begin, stringArray instArray);

void freePointers(command *instruction, stringArray instructionsArray);


#endif