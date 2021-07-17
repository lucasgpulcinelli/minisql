#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sqlInterpret.h"
#include "utils.h"

#define SIZE 1024
#define FUCK printf("FUCK: %i\n", __LINE__);

char** getSourceFiles(char **instArray){
    int start, end;
    int i = 0;

    while (instArray[i] != NULL)
    {
        if(!strcmp(instArray[i], FROM)){
            start = i;
        }
        if(!strcmp(instArray[i], WHERE)){
            end = i-1;
        }
        i++;
    }

    char **output = malloc(sizeof(char *) * end - start);

    int j = start + 1;
    i = 0;
    while (j <= end)
    {
        int stringSize = strlen(instArray[j]);
        for(int k = 0; k < stringSize; k++){
            if (instArray[j][k] == ',')
            {
                instArray[j][k] = '\0';
                stringSize--;
                break;
            }
        }

        output[i] = malloc(sizeof(char) * stringSize);
        strcpy(output[i], instArray[j]);
        j++;
        i++;
    }
    return output;
}

void getInstructions(){
    char *rawInstructions = malloc(sizeof(char) * SIZE);

    fgets(rawInstructions, SIZE, stdin);
    int allocationSize = strlen(rawInstructions);

    rawInstructions = realloc(rawInstructions, sizeof(char) * allocationSize);
    
    int ncols = get_ncols(rawInstructions);
    char **instArray = malloc(sizeof(char *) * ncols);

    separate_character(rawInstructions, ncols, instArray, " ");
    
    command *instruction = malloc(sizeof(instruction) * 1);

    instruction->from = getSourceFiles(instArray);

    int i = 0;
    while (instruction->from[i] != NULL)
    {
        printf("%s\n", instruction->from[i]);
        i++;
    }
    

    /*instruction->where = getConditions(rawInstructions);
    instruction->select = getSelection(rawInstructions);
    */
    free(rawInstructions);
}