#ifndef __UTILS_H__
#define __UTILS_H__

//pega uma linha separada por delim com cols valores e retorna esses valores separados em dest
//retorna != 0 em caso de erro 
int separate_character(char* line_buffer, unsigned int cols, char** dest, char* delim);

#endif