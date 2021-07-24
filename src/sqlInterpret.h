#ifndef __SQLINTERPRET_H__ 
#define __SQLINTERPRET_H__

#include "utils.h"
typedef struct {
    char *file_name;
    char *key;
} Member;

typedef struct {
    Member *place;
    char *comparation_value;
    Member *comparation_member;
} Condition;

typedef struct {
    StringArray from;
    Condition *where;
    int where_size;
    Member *select;
    int select_size;
} Command;

StringArray getInstructions(void);

Command *generateCommand(StringArray instructions_array);

Member *getSelection(StringArray inst_array, int *amount);

Condition *getConditions(StringArray inst_array, int *amount);

char** getSourceFiles(StringArray inst_array, int *number_of_files);

void isolateCommand(int *start_index, int *size, char *begin, StringArray inst_array);

//dealoca um command da memoria
void freeCommand(Command *instruction);

#endif