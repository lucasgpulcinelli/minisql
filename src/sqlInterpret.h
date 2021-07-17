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


typedef struct {
    char **from;
    condition *where;
    member *select;
}command;

void getInstructions();

#endif