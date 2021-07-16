
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "dataframe.h"

#define INIT_ROWS_MAXSIZE  32
#define FGETS_MAXLEN      256


//helper functions
long int read_header(char* line_buffer, char***keys);
int append_df_line(dataframe* df, char* line_buffer);
int separate_tabs(char* line_buffer, unsigned int cols, char** dest);


int separate_tabs(char* line_buffer, unsigned int cols, char** dest)
{
    int got_null = 0;
    char* last_tab = line_buffer;
    int i;
    for(i = 0; i < cols-1; i++, last_tab = strchr(last_tab, '\t')+1)
    {
        unsigned int size = (unsigned int) (strchr(last_tab, '\t') - last_tab);
        dest[i] = malloc(sizeof(char) * size);
        if(dest[i] == NULL)
        {
            got_null = 1;
            break;
        }
        strncpy(dest[i], last_tab, size);
    }

    if(got_null)
    {
        for(int j = 0; j < i; j++)
        {
            free(dest[i]);
        }
        return -1;
    }

    unsigned int size = (unsigned int) (strchr(last_tab, '\n') - last_tab);
    dest[cols-1] = malloc(sizeof(char) * size);
    if(dest[cols-1] == NULL)
    {
        for(int j = 0; j < cols-1; j++)
        {
            free(dest[i]);
        }
        return -1;
    }
    strncpy(dest[cols-1], last_tab, size);

    return 0;
}

long int read_header(char* line_buffer, char***keys)
{
    //descobre quantas colunas o header tem
    int cols = 1;
    char* last_tab = line_buffer;
    while((last_tab = strchr(last_tab, '\t')) != NULL)
    {
        last_tab++;
        cols++;
    }

    //cria as keys
    *keys = malloc(sizeof(char**) * cols);
    if(keys == NULL)
    {
        return -1;
    }

    int ret = separate_tabs(line_buffer, cols, *keys);
    if(ret != 0)
    {
        return -1;
    }

    return cols;
}

int append_df_line(dataframe* df, char* line_buffer)
{
    //realoca memoria se necessario
    if(df->_rows_maxsize < df->rows+1)
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
    }

    separate_tabs(line_buffer, df->cols, df->values[df->rows++]);
    return 0;
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

    while(fgets(line_buffer, FGETS_MAXLEN, fptr) != NULL)
    {
        append_df_line(df, line_buffer);
    }

    fclose(fptr);
    return df;
}

int append_df(dataframe* df, char** value)
{
    return -1;
}

void write_df(FILE* fptr, dataframe* df, int with_header)
{
    return;
}



void delete_many_dfs(dataframe** dflist, unsigned int size)
{
    for(int i = 0; i < size; i++)
    {
        delete_df(dflist[i]);
    }
    return;
}

void delete_df(dataframe* df)
{
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