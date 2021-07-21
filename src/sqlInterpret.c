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
    int allocation_size = strlen(raw_instructions);

    raw_instructions = realloc(raw_instructions, sizeof(char) * allocation_size + 1);
    
    StringArray inst_array;
    inst_array.size = getNcols(raw_instructions, ' ');
    
    inst_array.str = malloc(sizeof(char *) * inst_array.size);
    separateCharacter(raw_instructions, inst_array.size, inst_array.str, " ");

    free(raw_instructions);

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

    int i = 0;
    int final_index = start + *number_of_files;
    for (int j = start; j < final_index; j++){
        int aloc_size = strlen(inst_array.str[j]);
        output[i] = malloc(sizeof(char) * aloc_size + 1);
        xalloc(output[i])

        strcpy(output[i], inst_array.str[j]);

        removeChar(output[i], ',');
        removeChar(output[i], '\n');
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
        char **holder = malloc(sizeof(char *) * 2);
        xalloc(holder)

        removeChar(inst_array.str[j], ',');
        separateCharacter(inst_array.str[j], 2, holder, ".");

        output[i].file_name = holder[0];
        output[i].key = holder[1];

        free(holder);
    }

    return output;
}

Condition *getConditions(StringArray inst_array, int *amount){
    int start;
    isolateCommand(&start, amount, WHERE, inst_array);

    if(start < 0){
        *amount = 0;
        return NULL;
    }

    int final_index = start + *amount;
    *amount /= 3;

    Condition *output = malloc(*amount * sizeof(Condition));
    xalloc(output)

    for (int j = start, i = 0; j < final_index; j++, i++){
        char **holder = malloc(sizeof(char *) * 2);
        xalloc(holder)
        separateCharacter(inst_array.str[j], 2, holder, ".");

        output[i].place = malloc(sizeof(Member));
        xalloc(output[i].place)

        output[i].place->file_name = holder[0];
        output[i].place->key= holder[1];

        j += 2; //jumps the = sign
        removeChar(inst_array.str[j], ',');
        output[i].comparation_value = inst_array.str[j];

        free(holder);
    }

    return output;
}

void isolateCommand(int *start_index, int *size, char *COMMAND, StringArray inst_array){
    *start_index = -1;
    int i;
    for (i = 0; i < inst_array.size; i++){
        if (!strcmp(inst_array.str[i], COMMAND)){
            *start_index = i + 1;
            continue;
        }

        int is_select = !strcmp(inst_array.str[i], SELECT);
        int is_from = !strcmp(inst_array.str[i], FROM);
        int is_where = !strcmp(inst_array.str[i], WHERE);
        if ((is_select || is_from || is_where) && *start_index != -1){
            break;
        }
    }
    *size = i - *start_index;
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
    }
    free(instruction->where);

    free(instruction);
}