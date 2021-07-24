/*
Dataframe é responsável por armazenar as informações retiradas dos arquivos TSV
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "dataframe.h"
#include "utils.h"

#define INIT_ROWS_MAXSIZE  32
#define FGETS_MAXLEN      256


//funcoes usadas internamente:

/* 
separa as diferentes keys contidas em line_buffer e coloca em *keys
assume que *keys nao foi alocado (por nao saber a quantidade de colunas), por isso o ponteiro para essa variavel
retorna a quantidade de valores separados por tabs em keys (colunas do dataframe / tamanho de *keys) 
*/
int readHeader(char* line_buffer, char***keys);

//igual a append_df, mas usando uma linha tsv ao invez de uma coluna ja separada
int appendDfLine(DataFrame* df, char* line_buffer);

//escreve em fptr os valores de values (de tamanho size) separados por tab
void writeTsvRow(FILE* fptr, char** values, int size);

//aumenta o tamanho das rows de df
int dfIncreaseSize(DataFrame* df);


int readHeader(char* line_buffer, char*** keys){
    //descobre quantas colunas o header tem
    int cols = getNCols(line_buffer, '\t');

    //cria as keys
    *keys = malloc(sizeof(char**) * cols);
    xalloc(keys);

    separateCharacter(line_buffer, cols, *keys, "\t");

    return cols;
}

int dfIncreaseSize(DataFrame* df){
    char*** newvalues = realloc(df->values, sizeof(char**) * df->_rows_maxsize*2);
    xalloc(newvalues);

    df->values = newvalues;

    for(int i = df->_rows_maxsize; i < df->_rows_maxsize*2; i++){

        df->values[i] = malloc(sizeof(char*) * df->cols);
        xalloc(df->values[i]);
    }

    df->_rows_maxsize *= 2;

    return 0;
}

int appendDfLine(DataFrame* df, char* line_buffer){
    //realoca memoria se necessario
    if(df->_rows_maxsize < df->rows+1){        
        dfIncreaseSize(df);
    }

    return separateCharacter(line_buffer, df->cols, df->values[df->rows++], "\t");
}

void writeTsvRow(FILE* fptr, char** values, int size){

    for(int i = 0; i < size-1; i++){
        fprintf(fptr, "%s\t", values[i]);
    }
    fprintf(fptr, "%s\n", values[size-1]);
}

DataFrame* createDf(char** keys, int cols){
    DataFrame* df = malloc(sizeof(DataFrame));
    xalloc(df);

    df->name = NULL;
    df->keys = keys;
    df->rows = 0;
    df->cols = cols;
    df->_rows_maxsize = INIT_ROWS_MAXSIZE;

    df->values = malloc(sizeof(char**) * df->_rows_maxsize);
    xalloc(df->values);

    for(int i = 0; i < df->_rows_maxsize; i++){

        df->values[i] = malloc(sizeof(char*) * df->cols);
        xalloc(df->values[i]);
    }

    return df;
}

DataFrame** readManyDfs(char** filenames, int size){

    DataFrame** df_list = malloc(sizeof(DataFrame*) * size);
    xalloc(df_list);

    for(int i = 0; i < size; i++){
        df_list[i] = readDf(filenames[i]);
    }

    return df_list;
}

DataFrame* readDf(char* filename){
    //cria o nome do arquivo com .tsv no final
    char tsvfilename[strlen(filename)+1+4]; //+1 para '\0' e +4 para ".tsv"
    strcpy(tsvfilename, filename);
    strcat(tsvfilename, ".tsv");

    //abre o arquivo 
    FILE* fptr = fopen(tsvfilename, "r");
    if(fptr == NULL){
        abortProgram("File %s ", tsvfilename);
    }

    //pega a linha do header
    char line_buffer[FGETS_MAXLEN];
    if(fgets(line_buffer, FGETS_MAXLEN, fptr) == NULL){
        errno = EINVAL;
        abortProgram("File %s header ", tsvfilename);
    }
    line_buffer[strlen(line_buffer)-1] = '\0';

    //cria as keys
    char** keys;
    int cols = readHeader(line_buffer, &keys);
    
    //cria um df com essas keys
    DataFrame* df = createDf(keys, cols);
    
    //inicializa o nome
    df->name = malloc(sizeof(char) * (strlen(filename)+1));
    xalloc(df->name);

    strcpy(df->name, filename);

    //para cada linha no input ate o EOF coloca no dataframe
    while(fgets(line_buffer, FGETS_MAXLEN, fptr) != NULL){

        line_buffer[strlen(line_buffer)-1] = '\0';

        appendDfLine(df, line_buffer);
    }

    fclose(fptr);
    return df;
}

void appendDf(DataFrame* df, char** value){
    //realoca memoria se necessario
    if(df->_rows_maxsize < df->rows+1){        
        dfIncreaseSize(df);
    }

    for(int i = 0; i < df->cols; i++){

        df->values[df->rows][i] = malloc(sizeof(char) * (strlen(value[i])+1));
        xalloc(df->values[df->rows][i]);

        strcpy(df->values[df->rows][i], value[i]);
    }

    df->rows++;
}

char* dfAt(DataFrame* df, int row, char* key){

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

void writeDf(FILE* fptr, DataFrame* df){

    if(df == NULL){
        errno = EFAULT;
        abortProgram("df write ");
    }

    for(int i = 0; i < df->rows; i++){
       writeTsvRow(fptr, df->values[i], df->cols);
    }
}

void deleteManyDfs(DataFrame** dflist, int size){
    
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