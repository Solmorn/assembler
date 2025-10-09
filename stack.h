#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
// TODO: eto
//allocation funcs (done)
//canary fill func (done)
//canary flags (done)
//read about bite operators (done)
//constants (done)

//#define HASH_ON
//#define CANARY_ON

#ifndef STACK_H
#define STACK_H

typedef double stack_type;
typedef int    stack_error_code;

static const size_t     MAX_SIZE_VALUE     = 0x00011A6AAD;
static const size_t     MIN_SIZE_VALUE     =           10;
static const stack_type POISON             = 0x00000D1127;
static const size_t     STACK_EXPAND_VALUE =            2;
static const stack_type STACK_CANARY       = 0x00FA7B012D;
static const stack_type DATA_CANARY        = 0x00FA7B112D;
static const uint64_t   HASH_DEFAULT_VALUE =         5381;


enum Stack_Err_t {
    Ok                        =      0,//
    AllocationError           = 1 << 1,
    PopSizeStackError         = 1 << 2,
    StackCanaryError          = 1 << 3,
    NullptrDataError          = 1 << 4,
    SizeError                 = 1 << 5,//
    CapacityError             = 1 << 6,//
    SizeMoreThanCapacityError = 1 << 7,//
    PoisonDataError           = 1 << 8,//
    PoisonFillingError        = 1 << 9,//
    DataCanaryError           = 1 << 10,//
    HashError                 = 1 << 11,//
};

#ifdef _DEBUG

struct BirthInfo {
    const char* file;
    const char* func;
    const char* name;
    int         line;
};

#ifdef CANARY_ON
struct Canary_p {
    stack_type* data_canary1;
    stack_type* data_canary2;
};
#endif //canary

#define INIT_STACK(stk, capacity) do {                          \
    static BirthInfo stack_info_got = {__FILE__, __func__, #stk, __LINE__};  \
    StackCtor(&stk, capacity, &stack_info_got);                       \
} while(0)

#define ASSERT_OK(stk) do{              \
                                        \
    stack_error_code code = 0;                \
                                        \
    if ((code = StkErr(stk)) != 0) {    \
        StkDump(stk);                   \
        printf("ASSERTION FAILED\n\n"); \
        return code;                    \
    }                                   \
                                        \
}while(0)

#else //debug

#define INIT_STACK(stk, capacity) do {                          \
    StackCtor(&stk, capacity);                                  \
} while(0)

#define ASSERT_OK(stk) do{              \
}while(0)

#endif //debug



struct StackInfo {

    #ifdef _DEBUG
    stack_type stack_canary1  = STACK_CANARY;
    #endif

    stack_type*  data          = nullptr;
    size_t       size          = 0;
    size_t       capacity      = 0;

    #ifdef _DEBUG
    BirthInfo* stack_info           = nullptr;
    stack_error_code errors_bit     = 0;

    #ifdef CANARY_ON
    Canary_p   data_canary_p  = {};//
    #endif //canary

    #ifdef HASH_ON
    uint64_t hash_value       = HASH_DEFAULT_VALUE;
    #endif // HASH_ON

    #endif // _DEBUG

    #ifdef _DEBUG
    stack_type stack_canary2  = STACK_CANARY;
    #endif
};

#ifdef _DEBUG
stack_error_code StkErr(StackInfo* stk);
void StkDump(StackInfo* stk);
#ifdef HASH_ON
uint64_t CalculateDataHash(StackInfo* stk);
#endif //hash
bool ContainsError(stack_error_code code, Stack_Err_t err);
#endif




stack_error_code StackCtor(StackInfo* stk, size_t capacity, BirthInfo* stack_info_got);
stack_error_code StackPush(StackInfo* stk, stack_type element);
stack_error_code StackPop(StackInfo* stk, stack_type* element);
stack_error_code StkDtor(StackInfo* stk);

#endif
