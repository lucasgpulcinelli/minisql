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

//recebe as instruções do usuario
StringArray getInstructions(void);

//Gera uma struct de comando com todas as informações do usuario quebradas de forma elegante
Command *generateCommand(StringArray instructions_array);

//Pega quais campos devem ser selecionados pelo select
Member *getSelection(StringArray inst_array, int *amount);

//Pega as condições do Where
Condition *getConditions(StringArray inst_array, int *amount);

//Pega os arquivos do from que devem ser abertos
char** getSourceFiles(StringArray inst_array, int *number_of_files);

//isola um comando que está em um array: recebemos de volta por ponteiro a posição da primeira instrução do comando pedido e qnts instruções mais tem
void isolateCommand(int *start_index, int *size, char *COMMAND, StringArray inst_array);

//cria um ponteiro para um membro (file_name e key) a partir de uma instrução completa (file_name.key (Docentes.Nome))
Member *createMemberFromFull(const char *full);

//dealoca um command da memoria
void freeCommand(Command *instruction);

#endif