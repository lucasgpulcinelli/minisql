/* 
Utils são funções gerais que nos ajudam em várias partes do código e não tem um bom local definido
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>

#include "utils.h"
#include "sqlInterpret.h"

//funções internas

//remove um char de uma string em uma posição especifica
void removeAt(char *str, int index);


void fatalError(int line, char* file, char* fmt, ...){
    int init_errno = errno; //errno pode mudar de valor em vfprintf, entao guarda o valor inical

    va_list ap;

    va_start(ap, fmt);

    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "error at line %d of file %s: %s\n", line, file, strerror(init_errno));
    
    va_end(ap);

    exit(EXIT_FAILURE);
}

int separateCharacter(const char* line_buffer, int cols, char** dest, char* delim){

    //cria uma copia de line_buffer para não fazer alterações diretamente nele
    char *str = malloc(strlen(line_buffer) + 1); 
    xalloc(str);
    strcpy(str,line_buffer);    

    //cada call de strtok retorna o membro terminando com \0
    char* member = strtok(str, delim);

    for(int i = 0; i < cols; i++, member = strtok(NULL, delim)){

        dest[i] = malloc(sizeof(char) * (strlen(member)+1));
        xalloc(dest[i]);

        strcpy(dest[i], member);
    }

    free(str);

    return 0;
}

int getNCols(char *string, char delimiter){
    int cols = 1;
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
