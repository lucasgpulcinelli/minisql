#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sqlInterpret.h"
#include "dataframe.h"
#include "utils.h"

#define SIZE 1024

stringArray getInstructions()
{
    char *rawInstructions = malloc(sizeof(char) * SIZE);

    fgets(rawInstructions, SIZE, stdin);
    int allocationSize = strlen(rawInstructions);

    rawInstructions = realloc(rawInstructions, sizeof(char) * allocationSize + 1);

    show(rawInstructions);

    stringArray instArray;
    instArray.size = get_ncols(rawInstructions, ' ');
    
    instArray.str = malloc(sizeof(char *) * instArray.size);
    separate_character(rawInstructions, instArray.size, instArray.str, " ");

    free(rawInstructions);

    return instArray;
}

command *processInstructions(stringArray instructionsArray)
{
    command *instruction = malloc(sizeof(command) * 1);

    instruction->from.str = getSourceFiles(instructionsArray, &instruction->from.size);
    instruction->where = getConditions(instructionsArray, &instruction->whereSize);
    instruction->select = getSelection(instructionsArray, &instruction->selectSize);

    return instruction;
}

char **getSourceFiles(stringArray instArray, int *numberOfFiles)
{
    int start;
    isolateCommand(&start, numberOfFiles, FROM, instArray);

    char **output = malloc(*numberOfFiles * sizeof(char *));

    int i = 0;
    int finalIndex = start + *numberOfFiles;
    for (int j = start; j < finalIndex; j++)
    {
        int alocSize = strlen(instArray.str[j]);
        output[i] = malloc(sizeof(char) * alocSize + 1);
        strcpy(output[i], instArray.str[j]);

        removeChar(output[i], ',');
        removeChar(output[i], '\n');
        i++;
    }
    return output;
}

member *getSelection(stringArray instArray, int *amount)
{
    int start;
    isolateCommand(&start, amount, SELECT, instArray);

    member *output = malloc(*amount * sizeof(member));

    int i = 0;
    int finalIndex = start + *amount;
    for (int j = start; j < finalIndex; j++)
    {
        char **holder = malloc(sizeof(char *) * 2);
        removeChar(instArray.str[j], ',');
        separate_character(instArray.str[j], 2, holder, ".");

        output[i].fileName = holder[0];
        output[i].key = holder[1];

        free(holder);

        i++;
    }

    return output;
}

condition *getConditions(stringArray instArray, int *amount){
    int start;
    isolateCommand(&start, amount, WHERE, instArray);

    if(start < 0){
        *amount = 0;
        return NULL;}

    int finalIndex = start + *amount;
    *amount /= 3;

    condition *output = malloc(*amount * sizeof(condition));

    for (int j = start, i = 0; j < finalIndex; j++, i++)
    {
        char **holder = malloc(sizeof(char *) * 2);
        separate_character(instArray.str[j], 2, holder, ".");

        output[i].place = malloc(sizeof(member));

        output[i].place->fileName = holder[0];
        output[i].place->key= holder[1];

        j += 2; //jumps the = sign
        removeChar(instArray.str[j], ',');
        /*for(int k = 0; k < strlen(instArray.str[j]); k++){
            printf("%c\n", instArray.str[j][k]);
        }*/
        output[i].comparationValue = instArray.str[j];

        free(holder);
    }

    return output;
}

void isolateCommand(int *startIndex, int *size, char *COMMAND, stringArray instArray)
{
    //select X from Y where X
    //select X from Y

    *startIndex = -1;
    int i;
    for (i = 0; i < instArray.size; i++)
    {
        if (!strcmp(instArray.str[i], COMMAND))
        {
            *startIndex = i + 1;
            continue;
        }

        int isSelect = !strcmp(instArray.str[i], SELECT);
        int isFrom = !strcmp(instArray.str[i], FROM);
        int isWhere = !strcmp(instArray.str[i], WHERE);
        if ((isSelect || isFrom || isWhere) && *startIndex != -1)
        {
            break;
        }
    }
    *size = i - *startIndex;
}
