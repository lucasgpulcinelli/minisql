#ifndef __SQLDO_H__
#define __SQLDO_H__

#include "utils.h"
#include "dataframe.h"
#include "sqlInterpret.h"

//cria um dataframe com as instrucoes de SQL processadas
DataFrame* processCommand(Command* instruction);

#endif