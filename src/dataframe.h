#ifndef __DATAFRAME_H__ 
#define __DATAFRAME_H__

#include <stdio.h>

/*
membros:
name: nome do arquivo do dataframe (sem .tsv, ex: "Prog")
keys: chaves (ex: {Nome, Programa, Idioma, ...})
valores: valores na mesma ordem que chaves
    (ex: {{Lucas, SSC1234-1, Portugues, ...}, {Matheus, IAU0678-2, Espanhol}, ...})
rows: linhas no total
cols: colunas no total (mesmo tamanho que keys)

_rows_maxsize: tamanho maximo de linhas que o dataframe tem alocado (util para append_df)
*/
typedef struct{
    char* name; 
    char** keys; 
    char*** values; 
    int rows;
    int cols;
    int _rows_maxsize;
} DataFrame;

//cria dataframes vazios
DataFrame* createDf(char** keys, int cols);

//le arquivos .tsv de nome filename e retorna um dataframe com as informacoes
//em filename nao se deve colocar o .tsv no final
DataFrame** readManyDfs(char** filenames, int size);
DataFrame* readDf(char* filename);

//coloca o valor value no final do dataframe df
void appendDf(DataFrame* df, char** value);

//pega o valor de df com linha row e coluna key
char* dfAt(DataFrame* df, int row, char* key);

//coloca em fptr as informacoes do dataframe df
void writeDf(FILE* fptr, DataFrame* df);

//deleta dataframes
void deleteManyDfs(DataFrame** dflist, int size);
void deleteDf(DataFrame* df);

#endif