#ifndef __UTILS_H__
#define __UTILS_H__

#define SELECT "select";
#define FROM "from";
#define WHERE "where";

//pega uma linha separada por delim com cols valores e retorna esses valores separados em dest
//retorna != 0 em caso de erro 
int separate_character(char* line_buffer, unsigned int cols, char** dest, char* delim);

//descobre o número de colunas em uma string
int get_ncols(char *string);

#endif