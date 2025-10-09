#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <sys/stat.h>
#include <cstdlib>
#include <ctime>

#include "assembler.h"
#include "processor.h"

//system почитать про функцию


int main() {

    const char* commands_file = "commands.txt";
    const char* asm_file      = "assembler.txt";


    FileParams text_info = {};
    FillAssemblerFile(&text_info, asm_file, commands_file);


    Processor prc = {};
    INIT_PRC(prc, asm_file);
    RunAssembler(&prc);
    PrcDtor(&prc);


    printf("COMMIT ME! I WORK");

    return 0;
}


