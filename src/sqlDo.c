
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlDo.h"
#include "dataframe.h"


dataframe* processCommand(command* instruction)
{
    //cria o df com os arquivos do from
    dataframe** dfs = read_many_dfs(instruction->from.str, instruction->from.size);
    if(dfs == NULL)
    {
        return NULL;
    }
    
    //aloca as chaves
    char** out_keys = malloc(sizeof(char*) * instruction->selectSize);
    if(out_keys == NULL)
    {
        delete_many_dfs(dfs, instruction->from.size);
        return NULL;
    }

    int got_null = 0;
    int i;
    for(i = 0; i < instruction->selectSize; i++)
    {
        out_keys[i] = malloc(sizeof(char) * (strlen(instruction->select[i].key) + 1));
        if(out_keys[i] == NULL)
        {
            got_null = 1;
            break;
        }
        strcpy(out_keys[i], instruction->select[i].key);
    }

    if(got_null)
    {
        for(int j = 0; j < i; j++)
        {
            free(out_keys[i]);
        }
        free(out_keys);
        delete_many_dfs(dfs, instruction->from.size);
        return NULL;
    }

    //cria o df final
    dataframe* df_out = create_df(out_keys, instruction->selectSize);
    if(df_out == NULL)
    {
        for(int j = 0; j < i; j++)
        {
            free(out_keys[i]);
        }
        free(out_keys);
        delete_many_dfs(dfs, instruction->from.size);
    }


    char** rowvalues = malloc(sizeof(char *) * instruction->selectSize);
    if(rowvalues == NULL)
    {
        delete_df(df_out);
        delete_many_dfs(dfs, instruction->from.size);
        return NULL;
    }

    //aloca e seta as chaves do df_out
    for(int i = 0; i < dfs[0]->rows; i++)
    {
        for(int j = 0; j < instruction->selectSize; j++)
        {
            rowvalues[j] = df_at(dfs[0], i, instruction->select[j].key);
        }

        append_df(df_out, rowvalues);
    }

    free(rowvalues);
    delete_many_dfs(dfs, instruction->from.size);
    return df_out;
}
