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
typedef struct
{
    char* name; 
    char** keys; 
    char*** values; 
    unsigned int rows;
    unsigned int cols;
    unsigned int _rows_maxsize;
} dataframe;

//cria dataframes vazios
dataframe* create_df(char** keys, unsigned int cols);

//le arquivos .tsv de nome filename e retorna um dataframe com as informacoes
//em filename nao se deve colocar o .tsv no final
dataframe** read_many_dfs(char** filenames, unsigned int size);
dataframe* read_df(char* filename);

//coloca o valor value no final do dataframe df
int append_df(dataframe* df, char** value);

//coloca em fptr as informacoes do dataframe df, com o sem o header
void write_df(FILE* fptr, dataframe* df, int with_header);

//deleta dataframes
void delete_many_dfs(dataframe** dflist, unsigned int size);
void delete_df(dataframe* df);


#endif