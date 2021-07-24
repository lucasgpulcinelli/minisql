#include <stdio.h>

#include "sqlInterpret.h"
#include "utils.h"
#include "dataframe.h"
#include "sqlDo.h"

int main(int argc, char** argv)
{
    StringArray raw_instructions = getInstructions();
    Command *instruction = generateCommand(raw_instructions);
    
/*     for(int i = 0; i < instruction->where_size; i++){
        printf("%s.%s = %s ", instruction->where[i].place->file_name, instruction->where[i].place->key, instruction->where[i].comparation_value);
    }
    printf("\n"); */

    DataFrame* out_df = processCommand(instruction);

    writeDf(stdout, out_df, 0, instruction);

    deleteDf(out_df);

    freeCommand(instruction);
    freeStrArray(raw_instructions);
    return 0;
}
