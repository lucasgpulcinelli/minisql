#include <string.h>
#include <stdlib.h>

#include "utils.h"
#include "sqlInterpret.h"

int separateCharacter(char* line_buffer, unsigned int cols, char** dest, char* delim){
    int got_null = 0;
    int i;
    
    //cada call de strtok retorna o membro terminando com \0
    char* member = strtok(line_buffer, delim);

    for(i = 0; i < cols; i++, member = strtok(NULL, delim)){

        dest[i] = malloc(sizeof(char) * (strlen(member)+1));
        if(dest[i] == NULL){
            got_null = 1;
            break;
        }
        strcpy(dest[i], member);
    }

    if(got_null){
        for(int j = 0; j < i; j++){
            free(dest[i]);
        }
        return -1;
    }
    return 0;
}

unsigned int getNcols(char *string, char delimiter){
    unsigned int cols = 1;
    char* last_occurance = string;
    while((last_occurance = strchr(last_occurance, delimiter)) != NULL){
        last_occurance++; //pula o caractere delimitador
        cols++;
    }
    return cols;
}

void removeChar(char *str, char remove){
    int string_size = strlen(str);

    for(int k = 0; k < string_size; k++){
        if (str[k] == remove){
            str[k] = '\0';
            string_size--;
            break;
        }
    }
}

void freeStrArray(StringArray instructionsArray){
    for (int i = 0; i < instructionsArray.size; i++){
        free(instructionsArray.str[i]);
    }

    free(instructionsArray.str);
}

void freeCommand(Command *instruction){
    for (int i = 0; i < instruction->from.size; i++){
        free(instruction->from.str[i]);
    }
    free(instruction->from.str);

    for (int i = 0; i < instruction->selectSize; i++){
        free(instruction->select[i].fileName);
        free(instruction->select[i].key);
    }
    free(instruction->select);

    for (int i = 0; i < instruction->whereSize; i++){
        free(instruction->where[i].place->fileName);
        free(instruction->where[i].place->key);
        free(instruction->where[i].place);
    }
    free(instruction->where);

    free(instruction);
}