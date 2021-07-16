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

_maxsize: tamanhos maximos alocados (util para append)
*/
typedef struct
{
    char* name; 
    char** keys; 
    char*** values; 
    unsigned int rows;
    unsigned int cols;

    struct
    {
        unsigned int rows;
        unsigned int cols;
    } _maxsize;

} dataframe;

//le arquivos .tsv de nome filename e retorna um dataframe com as informacoes
//em filename nao se deve colocar o .tsv no final
dataframe** read_many_dfs(char** filenames);
dataframe* read_df(char* filename);

//coloca o valor value no final do dataframe df
int append_df(dataframe* df, char** value);

//coloca em fptr as informacoes do dataframe df, com o sem o header
dataframe* print_df(FILE* fptr, dataframe* df, int with_header);

//deleta dataframes
void delete_many_dfs(dataframe** dflist);
void delete_df(dataframe* df);


#endif