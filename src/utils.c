#include <string.h>
#include <stdlib.h>

#include "utils.h"

int separate_character(char* line_buffer, unsigned int cols, char** dest, char* delim)
{
    int got_null = 0;
    int i;
    
    //cada call de strtok retorna o membro terminando com \0
    char* member = strtok(line_buffer, delim);
    for(i = 0; i < cols; i++, member = strtok(NULL, delim))
    {
        dest[i] = malloc(sizeof(char) * (strlen(member)+1));
        if(dest[i] == NULL)
        {
            got_null = 1;
            break;
        }
        strcpy(dest[i], member);
    }

    if(got_null)
    {
        for(int j = 0; j < i; j++)
        {
            free(dest[i]);
        }
        return -1;
    }

    return 0;
}

int get_ncols(char *string){
    int cols = 0;
    int i = 0;

    while(string[i]!='\0')
    {
        /* check whether the current character is white space or new line or tab character*/
        if(string[i]==' ' || string[i]=='\n' || string[i]=='\t')
        {
            cols++;
        }

        i++;
    }

    return cols;
}

void removeChar(char *str, char remove){
    int stringSize = strlen(str);

    for(int k = 0; k < stringSize; k++){
        if (str[k] == remove)
        {
            str[k] = '\0';
            stringSize--;
            break;
        }
    }
}