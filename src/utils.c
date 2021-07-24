#include <string.h>
#include <stdlib.h>

#include "utils.h"
#include "sqlInterpret.h"

void removeAt(char *str, int index);

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
            if (str[k] == remove)
            {
                //str[k] = '\0';
                removeAt(str, k);
                string_size--;
            }
    }
}

void freeStrArray(StringArray instructionsArray){
    for (int i = 0; i < instructionsArray.size; i++){
        free(instructionsArray.str[i]);
    }

    free(instructionsArray.str);
}

void removeAt(char *str, int index){
    int stringSize = strlen(str);
    int amountOfPositionsToChange = (stringSize-index)-1; //quantas posições precisam ser mudadas no novo array, menos a ultima que será vazia

    for (int i = 0; i < amountOfPositionsToChange; i++)
    {
        str[index] = str[index+1]; //passa o valor da direita para a esquerda
        index++; //vai para o proximo valor
    }
    stringSize--; //diminui o tamanho do array
    str[stringSize] = '\0';
}

int stringHasChar(const char *str, char goal){
    int str_size = strlen(str);

    for(int i = 0; i < str_size; i++){
        if(str[i] == goal){
            return 1;
        }
    }
    return 0;
}

int ocurrencesInArray(StringArray arr, const char *val){
    int counter = 0;
    for(int i = 0; i < arr.size; i++){
        if(!strcmp(arr.str[i], val))
            counter++;
    }
    return counter;
}
