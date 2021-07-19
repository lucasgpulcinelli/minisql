#include <stdio.h>

#include "sqlInterpret.h"
#include "utils.h"
#include "dataframe.h"
#include "sqlDo.h"

int main(int argc, char** argv)
{
    stringArray rawInstructions = getInstructions();
    command *instruction = generateCommand(rawInstructions);

    dataframe* out_df = processCommand(instruction);
    
    write_df(stdout, out_df, 0);

    delete_df(out_df);
    freeCommand(instruction);
    freeStrArray(rawInstructions);
    return 0;
}
