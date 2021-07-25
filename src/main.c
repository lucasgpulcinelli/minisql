#include <stdio.h>
#include <stdlib.h>

#include "sqlInterpret.h"
#include "utils.h"
#include "dataframe.h"
#include "sqlDo.h"

int main(int argc, char** argv)
{
    //pega as instrucoes do usuario e gera o comando a partir delas
    StringArray raw_instructions = getInstructions();
    Command *instruction = generateCommand(raw_instructions);

    //processa o comando
    DataFrame* out_df = processCommand(instruction);
    
    //coloca na tela os resultados
    writeDf(stdout, out_df);

    deleteDf(out_df);
    freeCommand(instruction);
    freeStrArray(raw_instructions);
    exit(EXIT_SUCCESS);
}
