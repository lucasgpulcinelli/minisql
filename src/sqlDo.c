
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
    
    //aloca e seta as chaves
    char** out_keys = malloc(sizeof(char*) * instruction->select_size);
    if(out_keys == NULL){
        deleteManyDfs(dfs, instruction->from.size);
        return NULL;
    }

    int got_null = 0;
    int i;
    for(i = 0; i < instruction->select_size; i++){
        int out_size = strlen(instruction->select[i].file_name) + strlen(instruction->select[i].key) + 2;
        out_keys[i] = malloc(sizeof(char) * out_size);
        if(out_keys[i] == NULL){
            got_null = 1;
            break;
        }

        strcpy(out_keys[i], instruction->select[i].file_name);
        strcat(out_keys[i], ".");
        strcat(out_keys[i], instruction->select[i].key);
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
        return NULL;
    }


    char** row_values = malloc(sizeof(char *) * instruction->select_size);
    if(row_values == NULL){
        deleteDf(df_out);
        deleteManyDfs(dfs, instruction->from.size);
        return NULL;
    }

    //coloca os valores de df_out
    for(int i = 0; i < dfs[0]->rows; i++){
        //caso o where tenha sido passado e ele indique que a row nao deve ser colocada pula essa row
        if(instruction->where_size > 0 && !rowShould(instruction, dfs, i)){
            continue;    
        }

        for(int j = 0; j < instruction->select_size; j++){
            row_values[j] = dfAt(dfs[0], i, instruction->select[j].key);
        }
        appendDf(df_out, row_values);
    }

    free(row_values);
    deleteManyDfs(dfs, instruction->from.size);
    return df_out;
}

int rowShould(Command *instruction, DataFrame **dfs, int index){
    //Progs.Programa = Docentes.CodigodoPPG
    if(stringHasChar(instruction->where->comparation_value, '.')){
        char *holder = dfAt(dfs[0], index, instruction->where->place->key); //pega o que está na coluna linha para ser comparado
        return strcmp(holder, instruction->where->comparation_value) == 0; //compara os valores para analisar se aquela linha deve ser incluida
    }else{
        char *source = strtok(instruction->where->place->key, ".");
        int dataframe_index = getDfIndex(dfs, source, instruction->from.size);
        char *holder = dfAt(dfs[0], index, instruction->where->place->key); //pega o que está na coluna linha para ser comparado

        for(int i = 0; i < dfs[dataframe_index]->rows; i++){
            char *comparation_val = dfAt(dfs[dataframe_index], i, instruction->where->place->key);
            if(strcmp(holder, comparation_val) == 0)
                return 0;
        }
        return 1;
    }
}
