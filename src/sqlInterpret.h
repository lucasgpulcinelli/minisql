#ifndef __SQLINTERPRET_H__ 
#define __SQLINTERPRET_H__

#include "utils.h"

/* 
Field é usada para guardar campos de uma forma mais organizada
file_name.key
*/
typedef struct {
    char *file_name; //o nome do arquivo de origem desse campo
    char *key; //o campo que está sendo chamado
} Field;

/* 
Condition guarda uma condição do where
first_member é o que vem antes do igual
second_member é o que vem dps do igual, sendo que pode ser um 'term', que significa ser variavel ou um 'constant'
*/
typedef struct {
    Field *first_member_term; // é responsavel por guardar o membro da esquerda da comparação
    char *second_member_constant; //caso o membro da direita seja um valor constante ele é guardado aqui
    Field *second_member_term; //caso ele seja um campo ele é armazenado aqui como um Field
} Condition;

/* 
Command guarda diretamente os comandos passados pelo usuario
select X.Y from X where X.W = U vai ser salvo de forma organizada nessa string
*/
typedef struct {
    StringArray from; //armazena um StringArray com todos os arquivos pedidos no from
    Condition *where; //armazena um array de condições pedidas no where
    int where_size; //armazena quantas condições foram passadas
    Field *select; //armazena um array de campos que o usuario selecionou
    int select_size; //armazena quantos campos foram passados
} Command;

//recebe as instruções do usuario
StringArray getInstructions(void);

//Gera uma struct de comando com todas as informações do usuario quebradas de forma elegante
Command *generateCommand(StringArray instructions_array);

//Pega quais campos devem ser selecionados pelo select
Field *getSelection(StringArray inst_array, int *amount);

//Pega as condições do Where
Condition *getConditions(StringArray inst_array, int *amount);

//Pega os arquivos do from que devem ser abertos
char** getSourceFiles(StringArray inst_array, int *number_of_files);

//isola um comando que está em um array: recebemos de volta por ponteiro a posição da primeira instrução do comando pedido e qnts instruções mais tem
void isolateCommand(int *start_index, int *size, char *COMMAND, StringArray inst_array);

//cria um ponteiro para um membro (file_name e key) a partir de uma instrução completa (file_name.key (Docentes.Nome))
Field *createMemberFromFull(const char *full);

//dealoca um command da memoria
void freeCommand(Command *instruction);

#endif