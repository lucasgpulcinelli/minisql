#ifndef __UTILS_H__
#define __UTILS_H__

#define SELECT "select"
#define FROM "from"
#define WHERE "where"

#define xalloc(p) if(!p){fprintf(stderr, "ERROR: Allocation failed in line %i\n", __LINE__ - 1); exit(-1);} 

#define DEBUG printf("File: %s - Line: %i\n",__FILE__, __LINE__);
#define watch(var) printf("%i\n", var)
#define show(var) printf("%s\n", var)

//struct para administrar um array de strings
typedef struct{
    char **str;
    int size;
} StringArray;

//pega uma linha separada por delim com cols valores e retorna esses valores separados em dest
//aloca apenas os membros de dest (dest[0] ate dest[cols-1]), mas nao dest em si
//retorna != 0 em caso de erro 
int separateCharacter(char* line_buffer, unsigned int cols, char** dest, char* delim);

//descobre o numero de colunas em uma string com delimitador delim
unsigned int getNcols(char *string, char delimiter);

//remove um caracter especifico de uma string e subsitui por \0
void removeChar(char *str, char remove);

//desaloca um array de strings da memoria
void freeStrArray(StringArray instructionsArray);

//encontra a posição de um char em uma string, se esse char não estiver lá retorna 1
int stringHasChar(const char *str, char goal);

int ocurrencesInArray(StringArray arr, const char *val);

#endif