/* 
SQL Do é responsavel por filtrar os dados de acordo com as instruções passadas
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sqlDo.h"
#include "dataframe.h"
#include "utils.h"

//funções internas:

/*
analisa se a combinacao da row iout de df_out e iin de df_in deve ser incluida no dataframe final
considerando a condicao where de instruction
caso df_out seja NULL, analisa apenas o arquivo df_in
sempre considera que df_out tem formato Filename.Key e df_in nao esta
*/
int rowShould(Command *instruction, DataFrame *df_out, DataFrame* df_in, int iout, int iin);


DataFrame* processCommand(Command* instruction){
    //cria o df com os arquivos do from
    DataFrame** dfs = readManyDfs(instruction->from.str, instruction->from.size);
    
    //primeiro, copia dfs[0] para o df_out com o nome do arquivo nas chaves, para isso:
    //aloca e seta as chaves
    char** out_keys = malloc(sizeof(char*) * dfs[0]->cols);
    xalloc(out_keys);

    for(int i = 0; i < dfs[0]->cols; i++){

        out_keys[i] = malloc(sizeof(char) * (strlen(dfs[0]->keys[i]) +1+ strlen(dfs[0]->name)+ 1));
        xalloc(out_keys[i]);

        sprintf(out_keys[i], "%s.%s", dfs[0]->name, dfs[0]->keys[i]);
    }

    //cria o df final
    DataFrame* df_out = createDf(out_keys, dfs[0]->cols);

    char** row_values = malloc(sizeof(char *) * df_out->cols);
    xalloc(row_values);

    //coloca os valores de df_out
    for(int i = 0; i < dfs[0]->rows; i++){
        //caso o where tenha sido passado e ele indique que a row nao deve ser colocada pula essa row
        if(instruction->where_size > 0 && !rowShould(instruction, NULL, dfs[0], -1, i)){
            continue;    
        }

        for(int j = 0; j < df_out->cols; j++){
            row_values[j] = dfs[0]->values[i][j];
        }

        appendDf(df_out, row_values);
    }
    free(row_values);

    //depois disso, para cada um dos outros arquivos:
    //combina as rows de interesse de df_out e do outro arquivo e coloca num novo df caso 
    //o where permita, e por ultimo coloca esse novo dataframe em df_out para a proxima iteracao
    for(int i = 1; i < instruction->from.size; i++){

        
        int out_cols = df_out->cols + dfs[i]->cols;

        //aloca as chaves
        char** out_keys = malloc(sizeof(char*) * out_cols);
        xalloc(out_keys);

        //seta as novas chaves com tanto as do df_out quanto as do dfs[i]
        int j;
        for(j = 0; j < df_out->cols; j++){

            out_keys[j] = malloc(sizeof(char) * (strlen(df_out->keys[j])+1));
            xalloc(out_keys[j]);

            strcpy(out_keys[j], df_out->keys[j]);
        }

        for(int k = 0; k < dfs[i]->cols; k++, j++){

            out_keys[j] = malloc(sizeof(char) * (strlen(dfs[i]->keys[k]) +1+ strlen(dfs[i]->name)+ 1));
            xalloc(out_keys[j]);

            sprintf(out_keys[j], "%s.%s", dfs[i]->name, dfs[i]->keys[k]);
        }

        //cria o dataframe com as novas chaves
        DataFrame* new_df_out = createDf(out_keys, out_cols);

        char** row_values = malloc(sizeof(char *) * new_df_out->cols);
        xalloc(row_values);

        //para cada possivel combinacao de rows:
        for(int j = 0; j < dfs[i]->rows; j++){
            for(int k = 0; k < df_out->rows; k++){
                //caso o where tenha sido passado e ele indique que a row nao deve ser colocada pula essa combinacao
                if(instruction->where_size > 0 && !rowShould(instruction, df_out, dfs[i], k, j)){
                    continue;    
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
        df_out = new_df_out; //prepara a proxima iteracao
    }

    //no final, vamos ter todas as colunas de um dataframe processado com o where,
    //agora so falta pegar os valores do select especificos, para isso:

    //aloca e seta as chaves
    out_keys = malloc(sizeof(char*) * (instruction->select_size));
    xalloc(out_keys);

    for(int i = 0; i < instruction->select_size; i++){
        out_keys[i] = malloc(sizeof(char) * (strlen(instruction->select[i].file_name) + 1 + strlen(instruction->select[i].key)+ 1));
        xalloc(out_keys[i]);

        sprintf(out_keys[i], "%s.%s", instruction->select[i].file_name, instruction->select[i].key);
    }

    //cria o df final em si
    DataFrame* new_df_out = createDf(out_keys, instruction->select_size);

    //e para cada linha de df_out, copia o valor caso ele tenha chave em new_df_out
    row_values = malloc(sizeof(char*) * (new_df_out->cols));
    xalloc(row_values);
    
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


int rowShould(Command *instruction, DataFrame *df_out, DataFrame* df_in, int iout, int iin){
    
    //se nao tiver df_out so analisa o df_in
    if(df_out == NULL){
        df_out = df_in;
        iout = iin;
    }

    //para cada condicao
    for(int i = 0; i < instruction->where_size; i++){

        if(instruction->where[i].second_member_constant == NULL){
            //se for uma comparacao de variavel 

            int first_has_df_in = strcmp(df_in->name, instruction->where[i].first_member_term->file_name) == 0;

            //se o where[i] nao envolver o df_in pula
            if(!(first_has_df_in || strcmp(df_in->name, instruction->where[i].second_member_term->file_name) == 0)){
                continue;
            }

            //a sequencia file1.key1 = file2.key2 tem que dar o mesmo resultado que file1.key1 = file2.key2
            //para isso ve qual delas tem df_in
            Field df_in_member, df_out_member;
            if(first_has_df_in){
                df_in_member = *(instruction->where[i].first_member_term);
                df_out_member = *(instruction->where[i].second_member_term);
            }else{
                df_in_member = *(instruction->where[i].second_member_term);
                df_out_member = *(instruction->where[i].first_member_term);
            }

            //pega a chave inteira de df_out
            char full_key[strlen(df_out_member.file_name)+1+strlen(df_out_member.key)+1];
            
            if(df_out != df_in){
                sprintf(full_key, "%s.%s", df_out_member.file_name, df_out_member.key);
            }
            else{
                //caso o df_out seja igual a df_in, isso significa que o primeiro nao tem formato file.key
                strcpy(full_key, df_out_member.key);
            }

            //pega o valor da linha coluna de cada um dos dois e compara
            char* value_for_out = dfAt(df_out, iout, full_key);
            char* value_for_in = dfAt(df_in, iin, df_in_member.key);
            if(value_for_out == NULL){
                continue;
            }
            
            if(strcmp(value_for_out, value_for_in) != 0){
                return 0; //como o unico operador logico implementado foi o "and", qualquer comparacao falsa retorna imediatamente
            }

        }else{
            //se for uma comparacao com constante


            //ve se o arquivo se refere ao df_in, se nao pula esse where
            if(strcmp(df_in->name, instruction->where[i].first_member_term->file_name) != 0){
                continue;
            }
            
            //pega o valor constante e compara
            char* value_for_place = dfAt(df_in, iin, instruction->where[i].first_member_term->key);
            
            if(strcmp(value_for_place, instruction->where[i].second_member_constant) != 0){
                return 0; //como o unico operador logico implementado foi o "and", qualquer comparacao falsa retorna imediatamente
            }
        }
    }

    return 1; //o padrao e que a row deve ser incluida
}
