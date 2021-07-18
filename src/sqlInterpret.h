#ifndef __SQLINTERPRET_H__ 
#define __SQLINTERPRET_H__

typedef struct {
    char *fileName;
    char *key;
}member;

typedef struct {
    member *place;
    char *value;
}condition;

typedef struct{
    char **fileNames;
    int amount;
}files;

typedef struct {
    files from;
    condition *where;
    member *select;
}command;

char **getInstructions();

void freePointers(command *instruction, char **instructionsArray);

command *separateCommands(char **instructionsArray);

#endif