#ifndef PROCESSOR_H
#define PROCESSOR_H


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


#include "tools.h"
#include "stack.h"


struct Processor {
    double* code          = nullptr;
    double* code_cpy      = nullptr;
    double*  registers    = nullptr;
    StackInfo* stk        = nullptr;
};





void ProcessorCtor(Processor* prc, const char* asm_file);
void RunAssembler(Processor* prc, const char* asm_file);
void RunCode(Processor* prc);

#endif
