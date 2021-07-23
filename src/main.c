#include <stdio.h>

#include "sqlInterpret.h"
#include "utils.h"
#include "dataframe.h"
#include "sqlDo.h"

int main(int argc, char** argv)
{
    StringArray raw_instructions = getInstructions();
    Command *instruction = generateCommand(raw_instructions);

    DataFrame* out_df = processCommand(instruction);
    
    writeDf(stdout, out_df, 0, instruction);

    deleteDf(out_df);
    freeCommand(instruction);
    freeStrArray(raw_instructions);
    return 0;
}
