
#include "sqlDo.h"


dataframe* processCommand(command* instruction)
{
    printf("From files: \n");
    for (int i = 0; i < instruction->from.size; i++){
        printf("\t%s.tsv\n", instruction->from.str[i]);
    }
    printf("Where conditions are: \n");
    for (int i = 0; i < instruction->whereSize; i++){
        printf("\tField: %s in File: %s.tsv is equal to %s\n", instruction->where[i].place->key, instruction->where[i].place->fileName, instruction->where[i].comparationValue);
    }
    printf("Select: \n");
    for (int i = 0; i < instruction->selectSize; i++)
    {
        printf("\tFile: %s.tsv, Field: %s\n", instruction->select[i].fileName, instruction->select[i].key);
    }

    return NULL;
}
