#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sqlInterpret.h"
#include "dataframe.h"
#include "utils.h"

#define SIZE 1024

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

command *processInstructions(stringArray instructionsArray){
    command *instruction = malloc(sizeof(command) * 1);

    instruction->from.str = getSourceFiles(instructionsArray, &instruction->from.size);
    /*instruction->where = getConditions(rawInstructions);
    instruction->select = getSelection(rawInstructions);*/
    
    return instruction;
}

char** getSourceFiles(stringArray instArray, int *numberOfFiles){
    int start;
    isolateCommand(&start, numberOfFiles, FROM, instArray);

    char **output = malloc(*numberOfFiles * sizeof(char *));

    int i = 0;
    int finalIndex = start + *numberOfFiles;
    for(int j = start; j < finalIndex; j++){
        int alocSize = strlen(instArray.str[j]);
        output[i] = malloc(sizeof(char) * alocSize + 1);
        strcpy(output[i], instArray.str[j]);

        removeChar(output[i], ',');
        i++;
    }
    return output;
}

void isolateCommand(int *startIndex, int *size, char *COMMAND, stringArray instArray){
    //select X from Y where X
    //select X from Y

    *startIndex = -1;
    int i;
    for(i = 0; i < instArray.size; i++)
    {
        if(!strcmp(instArray.str[i], COMMAND)){
            *startIndex = i + 1;
            continue;
        }

        int isSelect = !strcmp(instArray.str[i], SELECT);
        int isFrom = !strcmp(instArray.str[i], FROM);
        int isWhere = !strcmp(instArray.str[i], WHERE);
        if((isSelect || isFrom || isWhere) && *startIndex != -1){           
            break;
        }
    }
    *size = i - *startIndex;
}

void freePointers(command *instruction, stringArray instructionsArray){
    for (int i = 0; i < instruction->from.size; i++)
    {
        free(instruction->from.str[i]);
    }
    free(instruction->from.str);
    free(instruction);

    for(int i = 0; i < instructionsArray.size; i++){ //this needs to be fixed because is a problem for valgrind
        free(instructionsArray.str[i]);
    };

    free(instructionsArray.str);
}