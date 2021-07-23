#ifndef __DATAFRAME_H__ 
#define __DATAFRAME_H__

#include <stdio.h>
#include "sqlInterpret.h"

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
    unsigned int rows;
    unsigned int cols;
    unsigned int _rows_maxsize;
} DataFrame;

//cria dataframes vazios
DataFrame* createDf(char** keys, unsigned int cols);

//le arquivos .tsv de nome filename e retorna um dataframe com as informacoes
//em filename nao se deve colocar o .tsv no final
DataFrame** readManyDfs(char** filenames, unsigned int size);
DataFrame* readDf(char* filename);

//coloca o valor value no final do dataframe df
int appendDf(DataFrame* df, char** value);

//pega o valor de df com linha row e coluna key
char* dfAt(DataFrame* df, unsigned int row, char* key);

//coloca em fptr as informacoes do dataframe df, com o sem o header
void writeDf(FILE* fptr, DataFrame* df, int with_header, Command *instruction);

//deleta dataframes
void deleteManyDfs(DataFrame** dflist, unsigned int size);
void deleteDf(DataFrame* df);


#endif