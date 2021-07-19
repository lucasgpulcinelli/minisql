#ifndef __UTILS_H__
#define __UTILS_H__

#define SELECT "select"
#define FROM "from"
#define WHERE "where"

#define DEBUG printf("File: %s - Line: %i\n",__FILE__, __LINE__);
#define watch(var) printf("%i\n", var)
#define show(var) printf("%s\n", var)

//struct para administrar um array de strings
typedef struct{
    char **str;
    int size;
}stringArray;

//pega uma linha separada por delim com cols valores e retorna esses valores separados em dest
//aloca apenas os membros de dest (dest[0] ate dest[cols-1]), mas nao dest em si
//retorna != 0 em caso de erro 
int separate_character(char* line_buffer, unsigned int cols, char** dest, char* delim);

//descobre o numero de colunas em uma string com delimitador delim
unsigned int get_ncols(char *string, char delimiter);

//remove um caracter especifico de uma string e subsitui por \0
void removeChar(char *str, char remove);

//desaloca um array de strings da memoria
void freeStrArray(stringArray instructionsArray);

#endif