
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "dataframe.h"
#include "utils.h"

#define INIT_ROWS_MAXSIZE  32
#define FGETS_MAXLEN      256


//funcoes usadas internamente:

//separa as diferentes keys contidas em line_buffer e coloca em *keys
//assume que *keys nao foi alocado (por nao saber a quantidade de colunas), por isso o ponteiro para essa variavel
//retorna a quantidade de valores separados por tabs em keys (colunas do dataframe / tamanho de *keys)
long int readHeader(char* line_buffer, char***keys);

//igual a append_df, mas usando uma linha tsv ao invez de uma coluna ja separada
int appendDfLine(DataFrame* df, char* line_buffer);

//escreve em fptr os valores de values (de tamanho size) separados por tab
void writeTsvRow(FILE* fptr, char** values, unsigned int size);

//aumenta o tamanho das rows de df
int dfIncreaseSize(DataFrame* df);


long int readHeader(char* line_buffer, char*** keys){
    //descobre quantas colunas o header tem
    unsigned int cols = getNcols(line_buffer, '\t');

    //cria as keys
    *keys = malloc(sizeof(char**) * cols);
    if(keys == NULL){
        return -1;
    }

    if(separateCharacter(line_buffer, cols, *keys, "\t")){
        return -1;
    }

    return cols;
}

int dfIncreaseSize(DataFrame* df){
    char*** newvalues = realloc(df->values, sizeof(char**) * df->_rows_maxsize*2);
    if(newvalues == NULL){
        return -1;
    }
    df->values = newvalues;

    int got_null = 0;
    unsigned int i;
    for(i = df->_rows_maxsize; i < df->_rows_maxsize*2; i++){

        df->values[i] = malloc(sizeof(char*) * df->cols);

        if(df->values[i] == NULL){
            got_null = 1;
            break;
        }
    }

    if(got_null){
        for(unsigned int j = df->_rows_maxsize; j < i; j++){
            free(df->values[j]);
        }
        return -1;
    }
    df->_rows_maxsize *= 2;

    return 0;
}

int appendDfLine(DataFrame* df, char* line_buffer){
    //realoca memoria se necessario
    if(df->_rows_maxsize < df->rows+1){        
        if(dfIncreaseSize(df)){
            return -1;
        }
    }

    return separateCharacter(line_buffer, df->cols, df->values[df->rows++], "\t");
}

void writeTsvRow(FILE* fptr, char** values, unsigned int size){

    for(int i = 0; i < size-1; i++){
        fprintf(fptr, "%s\t", values[i]);
    }
    fprintf(fptr, "%s\n", values[size-1]);
}


DataFrame* createDf(char** keys, unsigned int cols){
    DataFrame* df = malloc(sizeof(DataFrame));
    if(df == NULL){
        return NULL;
    }

    df->name = NULL;
    df->keys = keys;
    df->rows = 0;
    df->cols = cols;
    df->_rows_maxsize = INIT_ROWS_MAXSIZE;

    df->values = malloc(sizeof(char**) * df->_rows_maxsize);
    if(df->values == NULL){
        free(df);
        return NULL;
    }

    int got_null = 0;
    int i;
    for(i = 0; i < df->_rows_maxsize; i++){

        df->values[i] = malloc(sizeof(char*) * df->cols);
        if(df->values[i] == NULL){
            got_null = 1;
            break;
        }
    }

    if(got_null){
        for(int j = 0; j < i; j++){
            free(df->values[i]);
        }
        free(df->values);
        free(df);
        return NULL;
    }

    return df;
}

DataFrame** readManyDfs(char** filenames, unsigned int size){

    DataFrame** dflist = malloc(sizeof(DataFrame*) * size);
    if(dflist == NULL){
        return NULL;
    }

    int got_null = 0;
    int i;
    for(i = 0; i < size; i++){
        dflist[i] = readDf(filenames[i]);
        if(dflist[i] == NULL){
            got_null = 1;
            break;
        }
    }

    if(got_null){
        for(int j = 0; j < i; j++){
            deleteDf(dflist[i]);
        }
        free(dflist);
        return NULL;
    }

    return dflist;
}

DataFrame* readDf(char* filename){
    //cria o nome do arquivo com .tsv no final
    char tsvfilename[strlen(filename)+1+4]; //+1 para '\0' e +4 para ".tsv"
    strcpy(tsvfilename, filename);
    strcat(tsvfilename, ".tsv");

    //abre o arquivo 
    FILE* fptr = fopen(tsvfilename, "r");
    if(fptr == NULL){
        return NULL;
    }

    //pega a linha do header
    char line_buffer[FGETS_MAXLEN];
    if(fgets(line_buffer, FGETS_MAXLEN, fptr) == NULL){
        fclose(fptr);
        return NULL;
    }
    line_buffer[strlen(line_buffer)-1] = '\0';

    //cria as keys
    char** keys;
    int cols = readHeader(line_buffer, &keys);
    if(cols < 0){
        fclose(fptr);
        return NULL;
    }
    
    //cria um df com essas keys
    DataFrame* df = createDf(keys, cols);
    if(df == NULL){

        for(int i = 0; i < cols; i++){
            free(keys[i]);
        }
        free(keys);
        free(df);
        fclose(fptr);
        return NULL;   
    }
    
    //inicializa o nome
    df->name = malloc(sizeof(char) * (strlen(filename)+1));
    if(df->name == NULL){

        for(int i = 0; i < cols; i++){
            free(keys[i]);
        }
        free(keys);
        free(df);
        fclose(fptr);
        return NULL;
    }
    strcpy(df->name, filename);

    //para cada linha no input ate o EOF coloca no dataframe
    while(fgets(line_buffer, FGETS_MAXLEN, fptr) != NULL){

        line_buffer[strlen(line_buffer)-1] = '\0';

        if(appendDfLine(df, line_buffer)){
            deleteDf(df);
            fclose(fptr);
            return NULL;
        }
    }

    fclose(fptr);
    return df;
}

int appendDf(DataFrame* df, char** value){
    //realoca memoria se necessario
    if(df->_rows_maxsize < df->rows+1){        
        if(dfIncreaseSize(df)){
            return -1;
        }
    }

    int got_null = 0;
    int i;
    for(i = 0; i < df->cols; i++){

        df->values[df->rows][i] = malloc(sizeof(char) * (strlen(value[i])+1));
        if(df->values[df->rows][i] == NULL){
            got_null = 1;
            break;
        }
        strcpy(df->values[df->rows][i], value[i]);
    }

    if(got_null){
        for(int j = 0; j < i; j++){
            free(df->values[df->rows][i]);
        }
        return -1;
    }

    df->rows++;
    return 0;
}

char* dfAt(DataFrame* df, unsigned int row, char* key){

    if(row >= df->rows){
        return NULL;
    }

    for(int i = 0; i < df->cols; i++){
        if(strcmp(df->keys[i], key) == 0){
            return df->values[row][i];
        }
    }

    return NULL;
}

void writeDf(FILE* fptr, DataFrame* df, int with_header){

    if(df == NULL){
        fprintf(stderr, "Erro, call para write_df foi chamado com df NULL\n");
        return;
    }

    //no runcodes nao se pode colocar o header,
    //mas e tao facil e faz tanto sentido que eu preferi adicionar a opcao
    if(with_header){
        writeTsvRow(fptr, df->keys, df->cols);
    }

    for(int i = 0; i < df->rows; i++){
       writeTsvRow(fptr, df->values[i], df->cols);
    }
}

void deleteManyDfs(DataFrame** dflist, unsigned int size){
    
    for(int i = 0; i < size; i++){
        deleteDf(dflist[i]);
    }

    free(dflist);
    return;
}

void deleteDf(DataFrame* df){
    if(df == NULL){
        return;
    }
    
    for(int i = 0; i < df->cols; i++){
        free(df->keys[i]);
    }
    free(df->keys);

    for(int i = 0; i < df->rows; i++){
        for(int j = 0; j < df->cols; j++){
            free(df->values[i][j]);
        }
        free(df->values[i]);
    }

    //de rows ate _rows_maxsize o df tem memoria alocada para uma proxima row,
    //mas nao para a string dentro dela
    for(int i = df->rows; i < df->_rows_maxsize; i++){
        free(df->values[i]);
    }    

    free(df->values);

    if(df->name != NULL){
        free(df->name);
    }

    free(df);
    return;
}