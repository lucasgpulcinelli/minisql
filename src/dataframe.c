
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
long int read_header(char* line_buffer, char***keys);

//igual a append_df, mas usando uma linha tsv ao invez de uma coluna ja separada
int append_df_line(dataframe* df, char* line_buffer);

//escreve em fptr os valores de values (de tamanho size) separados por tab
void write_tsv_row(FILE* fptr, char** values, unsigned int size);

//aumenta o tamanho das rows de df
int df_increase_size(dataframe* df);


long int read_header(char* line_buffer, char*** keys)
{
    //descobre quantas colunas o header tem
    unsigned int cols = get_ncols(line_buffer, '\t');

    //cria as keys
    *keys = malloc(sizeof(char**) * cols);
    if(keys == NULL)
    {
        return -1;
    }

    if(separate_character(line_buffer, cols, *keys, "\t"))
    {
        return -1;
    }

    return cols;
}

int df_increase_size(dataframe* df)
{
    char*** newvalues = realloc(df->values, sizeof(char**) * df->_rows_maxsize*2);
    if(newvalues == NULL)
    {
        return -1;
    }
    df->values = newvalues;

    int got_null = 0;
    unsigned int i;
    for(i = df->_rows_maxsize; i < df->_rows_maxsize*2; i++)
    {
        df->values[i] = malloc(sizeof(char*) * df->cols);
        if(df->values[i] == NULL)
        {
            got_null = 1;
            break;
        }
    }

    if(got_null)
    {
        for(unsigned int j = df->_rows_maxsize; j < i; j++)
        {
            free(df->values[j]);
        }
        return -1;
    }
    df->_rows_maxsize *= 2;

    return 0;
}

int append_df_line(dataframe* df, char* line_buffer)
{
    //realoca memoria se necessario
    if(df->_rows_maxsize < df->rows+1)
    {        
        if(df_increase_size(df))
        {
            return -1;
        }
    }

    return separate_character(line_buffer, df->cols, df->values[df->rows++], "\t");
}

void write_tsv_row(FILE* fptr, char** values, unsigned int size)
{
    for(int i = 0; i < size-1; i++)
    {
        fprintf(fptr, "%s\t", values[i]);
    }
    fprintf(fptr, "%s\n", values[size-1]);
}


dataframe* create_df(char** keys, unsigned int cols)
{
    dataframe* df = malloc(sizeof(dataframe));
    if(df == NULL)
    {
        return NULL;
    }

    df->name = NULL;
    df->keys = keys;
    df->rows = 0;
    df->cols = cols;
    df->_rows_maxsize = INIT_ROWS_MAXSIZE;

    df->values = malloc(sizeof(char**) * df->_rows_maxsize);
    if(df->values == NULL)
    {
        free(df);
        return NULL;
    }

    int got_null = 0;
    int i;
    for(i = 0; i < df->_rows_maxsize; i++)
    {
        df->values[i] = malloc(sizeof(char*) * df->cols);
        if(df->values[i] == NULL)
        {
            got_null = 1;
            break;
        }
    }

    if(got_null)
    {
        for(int j = 0; j < i; j++)
        {
            free(df->values[i]);
        }
        free(df->values);
        free(df);
        return NULL;
    }

    return df;
}

dataframe** read_many_dfs(char** filenames, unsigned int size)
{
    dataframe** dflist = malloc(sizeof(dataframe*) * size);
    if(dflist == NULL)
    {
        return NULL;
    }

    int got_null = 0;
    int i;
    for(i = 0; i < size; i++)
    {
        dflist[i] = read_df(filenames[i]);
        if(dflist[i] == NULL)
        {
            got_null = 1;
            break;
        }
    }

    if(got_null)
    {
        for(int j = 0; j < i; j++)
        {
            delete_df(dflist[i]);
        }
        free(dflist);
        return NULL;
    }

    return dflist;
}

dataframe* read_df(char* filename)
{
    //cria o nome do arquivo com .tsv no final
    char tsvfilename[strlen(filename)+1+4]; //+1 para '\0' e +4 para ".tsv"
    strcpy(tsvfilename, filename);
    strcat(tsvfilename, ".tsv");

    //abre o arquivo 
    FILE* fptr = fopen(tsvfilename, "r");
    if(fptr == NULL)
    {
        return NULL;
    }

    //pega a linha do header
    char line_buffer[FGETS_MAXLEN];
    if(fgets(line_buffer, FGETS_MAXLEN, fptr) == NULL)
    {
        fclose(fptr);
        return NULL;
    }
    line_buffer[strlen(line_buffer)-1] = '\0';

    //cria as keys
    char** keys;
    int cols = read_header(line_buffer, &keys);
    if(cols < 0)
    {
        fclose(fptr);
        return NULL;
    }
    
    //cria um df com essas keys
    dataframe* df = create_df(keys, cols);
    if(df == NULL)
    {
        for(int i = 0; i < cols; i++)
        {
            free(keys[i]);
        }
        free(keys);
        free(df);
        fclose(fptr);
        return NULL;   
    }
    
    //inicializa o nome
    df->name = malloc(sizeof(char) * (strlen(filename)+1));
    if(df->name == NULL)
    {
        for(int i = 0; i < cols; i++)
        {
            free(keys[i]);
        }
        free(keys);
        free(df);
        fclose(fptr);
        return NULL;
    }
    strcpy(df->name, filename);

    //para cada linha no input ate o EOF coloca no dataframe
    while(fgets(line_buffer, FGETS_MAXLEN, fptr) != NULL)
    {
        line_buffer[strlen(line_buffer)-1] = '\0';
        if(append_df_line(df, line_buffer))
        {
            delete_df(df);
            fclose(fptr);
            return NULL;
        }
    }

    fclose(fptr);
    return df;
}

int append_df(dataframe* df, char** value)
{
    //realoca memoria se necessario
    if(df->_rows_maxsize < df->rows+1)
    {        
        if(df_increase_size(df))
        {
            return -1;
        }
    }

    int got_null = 0;
    int i;
    for(i = 0; i < df->cols; i++)
    {
        df->values[df->rows][i] = malloc(sizeof(char) * (strlen(value[i])+1));
        if(df->values[df->rows][i] == NULL)
        {
            got_null = 1;
            break;
        }
        strcpy(df->values[df->rows][i], value[i]);
    }

    if(got_null)
    {
        for(int j = 0; j < i; j++)
        {
            free(df->values[df->rows][i]);
        }
        return -1;
    }

    df->rows++;
    return 0;
}

char* df_at(dataframe* df, unsigned int row, char* key)
{
    if(row >= df->rows)
    {
        return NULL;
    }

    for(int i = 0; i < df->cols; i++)
    {
        if(strcmp(df->keys[i], key) == 0)
        {
            return df->values[row][i];
        }
    }

    return NULL;
}

void write_df(FILE* fptr, dataframe* df, int with_header)
{
    if(df == NULL)
    {
        fprintf(stderr, "Erro, call para write_df foi chamado com df NULL\n");
        return;
    }

    //no runcodes nao se pode colocar o header,
    //mas e tao facil e faz tanto sentido que eu preferi adicionar a opcao
    if(with_header)
    {
        write_tsv_row(fptr, df->keys, df->cols);
    }

    for(int i = 0; i < df->rows; i++)
    {
       write_tsv_row(fptr, df->values[i], df->cols);
    }
}

void delete_many_dfs(dataframe** dflist, unsigned int size)
{
    for(int i = 0; i < size; i++)
    {
        delete_df(dflist[i]);
    }
    free(dflist);
    return;
}

void delete_df(dataframe* df)
{
    if(df == NULL)
    {
        return;
    }
    
    for(int i = 0; i < df->cols; i++)
    {
        free(df->keys[i]);
    }
    free(df->keys);

    for(int i = 0; i < df->rows; i++)
    {
        for(int j = 0; j < df->cols; j++)
        {
            free(df->values[i][j]);
        }
        free(df->values[i]);
    }

    //de rows ate _rows_maxsize o df tem memoria alocada para uma proxima row,
    //mas nao para a string dentro dela
    for(int i = df->rows; i < df->_rows_maxsize; i++)
    {
        free(df->values[i]);
    }    

    free(df->values);

    if(df->name != NULL)
    {
        free(df->name);
    }

    free(df);
    return;
}