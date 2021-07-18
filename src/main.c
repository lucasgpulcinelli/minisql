#include <stdio.h>

#include "sqlInterpret.h"
#include "utils.h"
#include "dataframe.h"


int main(int argc, char** argv)
{
    stringArray rawInstructions = getInstructions();
    command *instruction = separateCommands(rawInstructions);
    return 0;
}
