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

    const char* filename = "commands.txt";
    const char* asm_file = "assembler.txt";

    size_t filesize = 0;
    GetFileSize(filename, &filesize);

    TextParams text_info = {};

    FillTextInfo(&text_info, filename, filesize);
    FillAssembler(&text_info, asm_file);

    Processor prc = {};
    StackInfo stack_for_processor = {};
    INIT_STACK(stack_for_processor, 10);


    prc.stk = &stack_for_processor;

    RunAssembler(&prc, asm_file);

    printf("COMMIT ME! I WORK");

    return 0;
}


