
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlDo.h"
#include "dataframe.h"

//descobre quantas colunas do DataFrame df estao no comando instruction
int selectColsInDf(Command* instruction, DataFrame* df){
    int out_cols = 0;
    for(int i = 0; i < instruction->sources_size; i++){
        if(strcmp(instruction->sources[i].file_name, df->name) == 0){
            out_cols++;
        }
    }
    return out_cols;
}

DataFrame* processCommand(Command* instruction){
    //cria o df com os arquivos do from
    DataFrame** dfs = readManyDfs(instruction->from.str, instruction->from.size);
    if(dfs == NULL){
        return NULL;
    }
    
    //primeiro, copia dfs[0] para o df_out, para isso:

    
    //ve quantas chaves do primeiro arquivo tem no select
    int out_cols = selectColsInDf(instruction, dfs[0]);

    //aloca e seta as chaves
    char** out_keys = malloc(sizeof(char*) * out_cols);
    if(out_keys == NULL){
        deleteManyDfs(dfs, instruction->from.size);
        return NULL;
    }

    //para cada coluna do out_keys:
    int got_null = 0;
    int i, j;
    for(i = 0, j = 0; i < out_cols; i++, j++){
        //ve qual coluna do dfs[0] que tem nome igual a um membro do select
        //(nao roda para sempre pois nesse caso ele ja teria saido do loop)
        while(strcmp(instruction->sources[j].file_name, dfs[0]->name) != 0){
            j++;
        }

        out_keys[i] = malloc(sizeof(char) * (strlen(instruction->sources[j].key) + 1));
        if(out_keys[i] == NULL){
            got_null = 1;
            break;
        }

        strcpy(out_keys[i], instruction->sources[j].key);
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
    DataFrame* df_out = createDf(out_keys, out_cols);
    if(df_out == NULL){
        for(int j = 0; j < i; j++){
            free(out_keys[i]);
        }
        free(out_keys);
        deleteManyDfs(dfs, instruction->from.size);
        return NULL;
    }

    char** row_values = malloc(sizeof(char *) * out_cols);
    if(row_values == NULL){
        deleteDf(df_out);
        deleteManyDfs(dfs, instruction->from.size);
        return NULL;
    }

    //coloca os valores de df_out
    for(int i = 0; i < dfs[0]->rows; i++){
        //caso o where tenha sido passado e ele indique que a row nao deve ser colocada pula essa row
        for(int j = 0; j < df_out->cols; j++){
            for(int k = 0; k < dfs[0]->cols; k++){

                if(strcmp(dfs[0]->keys[k], df_out->keys[j]) == 0){
                    row_values[j] = dfs[0]->values[i][k];
                    break;
                }
            }
        }
        appendDf(df_out, row_values);
    }
    free(row_values);

    //depois disso, para cada um dos outros arquivos:
    //combina as rows de interesse de df_out e do outro arquivo e coloca num novo df caso 
    //o where permita, e por ultimo coloca esse novo dataframe em df_out para a proxima iteracao
    for(int i = 1; i < instruction->from.size; i++){

        //descobre quantas colunas o novo df tem que ter
        out_cols = df_out->cols + selectColsInDf(instruction, dfs[i]);

        //aloca as chaves
        char** out_keys = malloc(sizeof(char*) * out_cols);
        if(out_keys == NULL){
            deleteDf(df_out);
            deleteManyDfs(dfs, instruction->from.size);
            return NULL;
        }

        //para cada coluna, seta a chave na sequencia do select
        int key_to_copy = 0;
        for(int j = 0; j < instruction->sources_size; j++){
            int use_key = 0;
            for(int k = 0; k <= i; k++){
                if(strcmp(instruction->sources[j].file_name, dfs[k]->name) == 0){
                    use_key = 1;
                    break;
                }
            }
            if(use_key){
                out_keys[key_to_copy] = malloc(sizeof(char) * (strlen(instruction->sources[j].key)+1));
                strcpy(out_keys[key_to_copy], instruction->sources[j].key);
                key_to_copy++;
            }
        }

        //cria o dataframe com as novas chaves
        DataFrame* new_df_out = createDf(out_keys, out_cols);


        char** row_values = malloc(sizeof(char *) * new_df_out->cols);
        if(row_values == NULL){
            deleteDf(new_df_out);
            deleteDf(df_out);
            deleteManyDfs(dfs, instruction->from.size);
            return NULL;
        }

        //para cada possivel combinacao de rows:
        for(int j = 0; j < dfs[i]->rows; j++){
            for(int k = 0; k < df_out->rows; k++){
                for(int m = 0; m < new_df_out->cols; m++){
                    char* value_to_copy = dfAt(df_out, k, new_df_out->keys[m]);
                    
                    if(value_to_copy != NULL){
                        row_values[m] = value_to_copy;
                    }
                }
                
                for(int m = 0; m < new_df_out->cols; m++){
                    char* value_to_copy = dfAt(dfs[i], j, new_df_out->keys[m]);
                    
                    if(value_to_copy != NULL){
                        row_values[m] = value_to_copy;
                    }
                }

                appendDf(new_df_out, row_values);
            }
        }


        free(row_values);
        deleteDf(df_out);
        df_out = new_df_out;
    }


    deleteManyDfs(dfs, instruction->from.size);
    return df_out;
}

/*int rowShould(Condition *where, DataFrame **dfs, int index){
    char *holder = dfAt(dfs[0], index, where->place->key); //pega o que está na coluna linha para ser comparado
    return strcmp(holder, where->comparation_value) == 0; //compara os valores para analisar se aquela linha deve ser incluida
}*/
