
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlDo.h"
#include "dataframe.h"


DataFrame* processCommand(Command* instruction){
    //cria o df com os arquivos do from
    DataFrame** dfs = readManyDfs(instruction->from.str, instruction->from.size);
    if(dfs == NULL){
        return NULL;
    }
    
    //aloca as chaves
    char** out_keys = malloc(sizeof(char*) * instruction->select_size);
    if(out_keys == NULL){
        deleteManyDfs(dfs, instruction->from.size);
        return NULL;
    }

    int got_null = 0;
    int i;
    for(i = 0; i < instruction->select_size; i++){
        out_keys[i] = malloc(sizeof(char) * (strlen(instruction->select[i].key) + 1));
        if(out_keys[i] == NULL){
            got_null = 1;
            break;
        }
        strcpy(out_keys[i], instruction->select[i].key);
    }

    if(got_null){
        for(int j = 0; j < i; j++){
            free(out_keys[i]);
        }
        free(out_keys);
        deleteManyDfs(dfs, instruction->from.size);
        return NULL;
    }

    //cria o df final
    DataFrame* df_out = createDf(out_keys, instruction->select_size);
    if(df_out == NULL){
        for(int j = 0; j < i; j++){
            free(out_keys[i]);
        }
        free(out_keys);
        deleteManyDfs(dfs, instruction->from.size);
    }


    char** rowvalues = malloc(sizeof(char *) * instruction->select_size);
    if(rowvalues == NULL){
        deleteDf(df_out);
        deleteManyDfs(dfs, instruction->from.size);
        return NULL;
    }

    //aloca e seta as chaves do df_out
    for(int i = 0; i < dfs[0]->rows; i++)
    {
        for(int j = 0; j < instruction->select_size; j++)
        {
            rowvalues[j] = dfAt(dfs[0], i, instruction->select[j].key);
        }

        appendDf(df_out, rowvalues);
    }

    free(rowvalues);
    deleteManyDfs(dfs, instruction->from.size);
    return df_out;
}
