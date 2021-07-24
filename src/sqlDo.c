
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
    
    //primeiro, copia dfs[0] para o df_out com o nome do arquivo nas chaves, para isso:

    //aloca e seta as chaves
    char** out_keys = malloc(sizeof(char*) * dfs[0]->cols);
    if(out_keys == NULL){
        deleteManyDfs(dfs, instruction->from.size);
        return NULL;
    }

    int got_null = 0;
    int i;
    for(i = 0; i < dfs[0]->cols; i++){

        out_keys[i] = malloc(sizeof(char) * (strlen(dfs[0]->keys[i]) +1+ strlen(dfs[0]->name)+ 1));
        if(out_keys[i] == NULL){
            got_null = 1;
            break;
        }

        sprintf(out_keys[i], "%s.%s", dfs[0]->name, dfs[0]->keys[i]);
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
    DataFrame* df_out = createDf(out_keys, dfs[0]->cols);
    if(df_out == NULL){
        for(int j = 0; j < i; j++){
            free(out_keys[i]);
        }
        free(out_keys);
        deleteManyDfs(dfs, instruction->from.size);
        return NULL;
    }


    char** row_values = malloc(sizeof(char *) * df_out->cols);
    if(row_values == NULL){
        deleteDf(df_out);
        deleteManyDfs(dfs, instruction->from.size);
        return NULL;
    }

    //coloca os valores de df_out
    for(int i = 0; i < dfs[0]->rows; i++){
        //caso o where tenha sido passado e ele indique que a row nao deve ser colocada pula essa row
        if(instruction->where_size > 0 && !rowShould(instruction->where, dfs, i)){
            continue;    
        }

        for(int j = 0; j < df_out->cols; j++){
            row_values[j] = dfs[0]->values[i][j];
        }

        appendDf(df_out, row_values);
    }
    free(row_values);


    got_null = 0;
    //depois disso, para cada um dos outros arquivos:
    //combina as rows de interesse de df_out e do outro arquivo e coloca num novo df caso 
    //o where permita, e por ultimo coloca esse novo dataframe em df_out para a proxima iteracao
    for(int i = 1; i < instruction->from.size; i++){

        //descobre quantas colunas o novo df tem que ter
        int out_cols = df_out->cols + dfs[i]->cols;

        //aloca as chaves
        char** out_keys = malloc(sizeof(char*) * out_cols);
        if(out_keys == NULL){
            got_null = 1;
            break;
        }

        //seta as novas chaves com tanto as do df_out quanto as do dfs[i]
        int j;
        for(j = 0; j < df_out->cols; j++){

            out_keys[j] = malloc(sizeof(char) * (strlen(df_out->keys[j])+1));
            if(out_keys[j] == NULL){
                got_null = 1;
                break;
            }

            strcpy(out_keys[j], df_out->keys[j]);
        }

        if(got_null){
            for(int k = 0; k < j; k++){
                free(out_keys[k]);
            }
            free(out_keys);
            break;
        }

        for(int k = 0; k < dfs[i]->cols; k++, j++){

            out_keys[j] = malloc(sizeof(char) * (strlen(dfs[i]->keys[k]) +1+ strlen(dfs[i]->name)+ 1));
            if(out_keys[j] == NULL){
                got_null = 1;
                break;
            }

            sprintf(out_keys[j], "%s.%s", dfs[i]->name, dfs[i]->keys[k]);
        }

        if(got_null){
            for(int k = 0; k < j; k++){
                free(out_keys[k]);
            }
            free(out_keys);
            break;
        }

        //cria o dataframe com as novas chaves
        DataFrame* new_df_out = createDf(out_keys, out_cols);
        if(new_df_out == NULL){
            got_null = 1;
            for(int k = 0; k < out_cols; k++){
                free(out_keys[k]);
            }
            free(out_keys);
            break;
        }


        char** row_values = malloc(sizeof(char *) * new_df_out->cols);
        if(row_values == NULL){
            got_null = 1;
            deleteDf(new_df_out);
            break;
        }

        //para cada possivel combinacao de rows:
        for(int j = 0; j < dfs[i]->rows; j++){
            for(int k = 0; k < df_out->rows; k++){
                //caso o where tenha sido passado e ele indique que a row nao deve ser colocada pula essa combinacao
                if(instruction->where_size > 0 && !rowShould(instruction->where, dfs, i)){
                    //continue;    
                }

                //copia os valores da linha para o df_out e dfs[i]
                int l;
                for(l = 0; l < df_out->cols; l++){
                    row_values[l] = df_out->values[k][l];
                }

                for(int m = 0; m < dfs[i]->cols; m++, l++){
                    row_values[l] = dfs[i]->values[j][m];
                }


                appendDf(new_df_out, row_values);
            }
        }


        free(row_values);
        deleteDf(df_out);
        df_out = new_df_out;
    }

    if(got_null){
        deleteDf(df_out);
        deleteManyDfs(dfs, instruction->from.size);
        return NULL;
    }

    //no final, vamos ter todas as colunas de um dataframe processado com o where,
    //agora so falta pegar os valores do select especificos

    out_keys = malloc(sizeof(char*) * (instruction->select_size));
    if(out_keys == NULL) {
        deleteDf(df_out);
        deleteManyDfs(dfs, instruction->from.size);
        return NULL;
    }

    got_null = 0;
    for(i = 0; i < instruction->select_size; i++)
    {
        out_keys[i] = malloc(sizeof(char) * (strlen(instruction->select[i].file_name) + 1 + strlen(instruction->select[i].key)+ 1));
        if(out_keys == NULL){
            got_null = 1;
            break;
        }

        sprintf(out_keys[i], "%s.%s", instruction->select[i].file_name, instruction->select[i].key);
    }

    if(got_null){
        for(int j = 0; j < i; j++){
            free(out_keys[j]);
        }
        free(out_keys);
        deleteManyDfs(dfs, instruction->from.size);
        deleteDf(df_out);
        return NULL;
    }

    DataFrame* new_df_out = createDf(out_keys, instruction->select_size);
    if(new_df_out == NULL){
        for(int i = 0; i < instruction->select_size; i++){
            free(out_keys[i]);
        }
        free(out_keys);
        deleteManyDfs(dfs, instruction->from.size);
        deleteDf(df_out);
        return NULL;
    }

    row_values = malloc(sizeof(char*) * (new_df_out->cols));
    if(row_values == NULL){
        deleteManyDfs(dfs, instruction->from.size);
        deleteDf(df_out);
        deleteDf(new_df_out);
        return NULL;
    }

    for(int i = 0; i < df_out->rows; i++){
        for(int j = 0; j < new_df_out->cols; j++){
            row_values[j] = dfAt(df_out, i, new_df_out->keys[j]);
        }
        appendDf(new_df_out, row_values);
    }

    free(row_values);
    deleteDf(df_out);
    deleteManyDfs(dfs, instruction->from.size);

    return new_df_out;
}

int rowShould(Condition *where, DataFrame **dfs, int index){
    char *holder = dfAt(dfs[0], index, where->place->key); //pega o que está na coluna linha para ser comparado
    return strcmp(holder, where->comparation_value) == 0; //compara os valores para analisar se aquela linha deve ser incluida
}
