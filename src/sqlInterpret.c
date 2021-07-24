#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sqlInterpret.h"
#include "dataframe.h"
#include "utils.h"

#define SIZE 1024

StringArray getInstructions(void){
    char *raw_instructions = malloc(sizeof(char) * SIZE);
    fgets(raw_instructions, SIZE, stdin);
    removeChar(raw_instructions, '\n');
    
    StringArray inst_array;
    inst_array.size = getNcols(raw_instructions, ' ');
    inst_array.str = malloc(inst_array.size * sizeof(char *));
    separateCharacter(raw_instructions, inst_array.size, inst_array.str, " ");

    free(raw_instructions);

    //limpa as strings de caracteres especiais
    for(int i = 0; i < inst_array.size; i++){
        removeChar(inst_array.str[i], ',');
        removeChar(inst_array.str[i], '\"');
    }

    return inst_array;
}

Command *generateCommand(StringArray instructions_array){
    Command *instruction = malloc(sizeof(Command) * 1);
    xalloc(instruction)

    instruction->from.str = getSourceFiles(instructions_array, &instruction->from.size);
    instruction->where = getConditions(instructions_array, &instruction->where_size);
    instruction->select = getSelection(instructions_array, &instruction->select_size);

    return instruction;
}

char **getSourceFiles(StringArray inst_array, int *number_of_files){
    int start;
    isolateCommand(&start, number_of_files, FROM, inst_array);

    char **output = malloc(*number_of_files * sizeof(char *));
    xalloc(output)

    //j é usado para dar o loop no array de instruções (start até (start + size))
    //i é usado para dar loop no array de arquivos (0 até size), n precisa ser limitado em cima pq o do j consegue limita-lo
    int i = 0;
    int final_index = start + *number_of_files;
    for (int j = start; j < final_index; j++){
        int aloc_size = strlen(inst_array.str[j]);
        output[i] = malloc(sizeof(char) * aloc_size + 1);
        xalloc(output[i])

        strcpy(output[i], inst_array.str[j]);
        i++;
    }
    return output;
}

Member *getSelection(StringArray inst_array, int *amount){
    int start;
    isolateCommand(&start, amount, SELECT, inst_array);

    Member *output = malloc(*amount * sizeof(Member));
    xalloc(output)

    int final_index = start + *amount;
    for (int i = 0, j = start; j < final_index; i++, j++){
        Member *holder = createMemberFromFull(inst_array.str[j]); //utiliza um holder pois output[i] não é um pointer
        xalloc(holder);

        output[i] = *holder;
        free(holder);
    }

    return output;
}

Condition *getConditions(StringArray inst_array, int *amount){
    int start;
    isolateCommand(&start, amount, WHERE, inst_array);

    //se o start for menor que 0 isso qr dizer q não tem WHERE
    if(start < 0){
        *amount = 0;
        return NULL;
    }

    int final_index = start + *amount;
    *amount /= 4; //todo o comando tem uma string, um igual, outra string e um "and" ou "or"
    *amount += 1; //exceto o ultimo que nao tem o "and" ou "or"

    Condition *output = malloc(*amount * sizeof(Condition));
    xalloc(output)

    for (int j = start, i = 0; j < final_index; j++, i++){
        output[i].place = createMemberFromFull(inst_array.str[j]);

        j += 2; //pula o "=" e vai para o proximo  
        
        if(strchr(inst_array.str[j], '.') == NULL){
            //se a string não tiver um . a gente sabe que ela e de comparacao constante
            output[i].comparation_value = inst_array.str[j];
        }else{
            //coloca o comparation_value como NULL para que quando necessario possamos descobrir se a condição é constante ou variavel
            output[i].comparation_value = NULL;
            output[i].comparation_member = createMemberFromFull(inst_array.str[j]);
        }

        j++; //pula o "and"
    }

    return output;
}

void isolateCommand(int *start_index, int *size, char *COMMAND, StringArray inst_array){
    *start_index = -1; //inicializa start_index como -1
    int i;
    for (i = 0; i < inst_array.size; i++){
        if (!strcmp(inst_array.str[i], COMMAND)){ //caso ele encontre o comando altera o start_index para ser a instrução seguinte
            *start_index = i + 1;
            continue;
        }

        int is_select = !strcmp(inst_array.str[i], SELECT);
        int is_from = !strcmp(inst_array.str[i], FROM);
        int is_where = !strcmp(inst_array.str[i], WHERE);
        if ((is_select || is_from || is_where) && *start_index != -1){ //se já tivermos encontrado a posição inicial do comando e encontramors algum outro comando
            break;
        }
    }
    *size = i - *start_index;
}

Member *createMemberFromFull(const char *full){
    char *str = malloc(strlen(full) + 1);
    strcpy(str,full); // faz uma copia do full para não altera-lo

    Member *output = malloc(sizeof(Member));
    xalloc(output)

    char* dot_i = strchr(str, '.'); //pega o ponteiro de onde está o '.' da string

    output->key = malloc(strlen(dot_i+1) + 1); //cria uma key que vai da string que inicia dps do '.' até o '/0'
    strcpy(output->key,dot_i+1); //copia essa string de '.'+1 até '/0' para a key

    *dot_i = '\0'; //transforma o '.' em '/0' encurtando a string str

    output->file_name = malloc(strlen(str) + 1); //cria um file name q do tamanho de str cortado até o '.'
    strcpy(output->file_name,str); //como str só vai até o '.' ele é igual ao file_name

    free(str); //libera essa copia do full que está toda quebrada

    return output;
}

void freeCommand(Command *instruction){
    for (int i = 0; i < instruction->from.size; i++){
        free(instruction->from.str[i]);
    }
    free(instruction->from.str);

    for (int i = 0; i < instruction->select_size; i++){
        free(instruction->select[i].file_name);
        free(instruction->select[i].key);
    }
    free(instruction->select);

    for (int i = 0; i < instruction->where_size; i++){
        free(instruction->where[i].place->file_name);
        free(instruction->where[i].place->key);
        free(instruction->where[i].place);

        if(instruction->where[i].comparation_value == NULL){
            free(instruction->where[i].comparation_member->file_name);
            free(instruction->where[i].comparation_member->key);
            free(instruction->where[i].comparation_member);
        }
    }
    free(instruction->where);

    free(instruction);
}
