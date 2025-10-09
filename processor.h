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

typedef int prc_error_code;

static const size_t NUMBER_OF_REGISTERS = 12;

#ifdef _DEBUG

//birthInfo from stack

#define INIT_PRC(prc, asm_file) do {                          \
    static BirthInfo prc_info_got = {__FILE__, __func__, #prc, __LINE__};  \
    PrcCtor(&prc, asm_file, &prc_info_got);                       \
} while(0)

#define PRC_ASSERT_OK(prc) do{              \
                                        \
    prc_error_code code = 0;                \
                                        \
    if ((code = PrcErr(prc)) != 0) {    \
        PrcDump(prc);                   \
        printf("PRC ASSERTION FAILED\n\n"); \
        return code;                    \
    }                                   \
                                        \
}while(0)

#else //debug

#define INIT_PRC(prc) do {                          \
    PrcCtor(&prc);                                  \
} while(0)

#define PRC_ASSERT_OK(prc) do{              \
}while(0)

#endif //debug


enum struct PrcErr_t {
    Ok                        = 0,
    RunningCodeAlocationError = 1 << 1,
    StackError                = 1 << 2,
    UnknownCommandError       = 1 << 3,
    PrcNullPtr                = 1 << 4
};


struct Processor {
    double* current_command_ptr            = nullptr;
    double* running_code_ptr               = nullptr;
    double  registers[NUMBER_OF_REGISTERS] = {};
    StackInfo* stk                         = nullptr;

    #ifdef _DEBUG
    BirthInfo* init_info                   = nullptr;
    prc_error_code err_code                = 0;
    #endif
};


void RunAssembler(Processor* prc);
void PrcCtor(Processor* prc, const char* asm_file, BirthInfo* prc_info_got);
void PrcDtor(Processor* prc);


#endif
