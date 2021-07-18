#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sqlInterpret.h"
#include "dataframe.h"
#include "utils.h"

#define SIZE 1024
#define FUCK printf("FUCK: %i\n", __LINE__);
#define watch(var) printf("%i\n", var)
#define show(var) printf("%s\n", var)

stringArray getInstructions(){
    char *rawInstructions = malloc(sizeof(char) * SIZE);

    fgets(rawInstructions, SIZE, stdin);
    int allocationSize = strlen(rawInstructions);

    rawInstructions = realloc(rawInstructions, sizeof(char) * allocationSize + 1);
    
    stringArray instArray; 
    instArray.size = get_ncols(rawInstructions);
    instArray.str = malloc(sizeof(char *) * instArray.size);
    separate_character(rawInstructions, instArray.size, instArray.str, " ");

    free(rawInstructions);

    return instArray;
}

command *separateCommands(stringArray instructionsArray){
    command *instruction = malloc(sizeof(command) * 1);

    instruction->from.str = getSourceFiles(instructionsArray.str, &instruction->from.size);
    /*instruction->where = getConditions(rawInstructions);
    instruction->select = getSelection(rawInstructions);*/

    freePointers(instruction, instructionsArray);
}

char** getSourceFiles(char **instArray, int *numberOfFiles){
    int start;
    commandGap(&start, numberOfFiles, FROM, WHERE, instArray);

    char **output = malloc(*numberOfFiles * sizeof(char *));

    int i = 0;
    int finalIndex = start + *numberOfFiles;
    for(int j = start; j < finalIndex; j++){
        int alocSize = strlen(instArray[j]);
        output[i] = malloc(sizeof(char) * alocSize + 1);
        strcpy(output[i], instArray[j]);

        removeChar(output[i], ',');
        i++;
    }
    return output;
}

void commandGap(int *startIndex, int *size, char *begin, char *final, char **instArray){
    int i = 0;

    while (instArray[i] != NULL)
    {
        if(!strcmp(instArray[i], begin)){
            *startIndex = i + 1;
        }
        if(!strcmp(instArray[i], final)){           
            break;
        }
        i++;
    }
    *size = i - *startIndex;
}

void freePointers(command *instruction, stringArray instructionsArray){
    for (int i = 0; i < instruction->from.size; i++)
    {
        free(instruction->from.str[i]);
        i++;
    }
    free(instruction->from.str);
    free(instruction);

    for(int i = 0; i < instructionsArray.size; i++){ //this needs to be fixed because is a problem for valgrind
        free(instructionsArray.str[i]);
    };

    free(instructionsArray.str);
}