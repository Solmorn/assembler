#include "processor.h"


#define PRC_ARITHMETIC(prc, operator) do{ \
    double a = 0;  \
    double b = 0; \
    StackPop(prc->stk, &a); \
    StackPop(prc->stk, &b); \
    StackPush(prc->stk, b operator a); \
}while(0)   \

#define PRC_CONDITION_JUMP(prc, operator) do{ \
    double a = 0;  \
    double b = 0;  \
    StackPop(prc->stk, &a);  \
    StackPop(prc->stk, &b);   \
    if (a operator b) prc->current_command_ptr = prc->running_code_ptr + (int)*(prc->current_command_ptr + 1); \
    else prc->current_command_ptr += 2; /*bad*/  \
}while(0) \


static void PrcHLT(Processor* prc) {
    assert(prc);
    printf("PROGRAM_ENDED_OK\n\n");
}
static void PrcOUT(Processor* prc) {
    assert(prc);
    double a = 0;
    StackPop(prc->stk, &a);
    printf("%lf\n", a);
}
static void PrcIN(Processor* prc) {
    assert(prc);
    double a = 0;
    scanf("%lf", &a);
    StackPush(prc->stk, a);
}
static void PrcPOPR(Processor* prc) {
    assert(prc);
    double a = 0;
    StackPop(prc->stk, &a);
    prc->registers[(int)*(prc->current_command_ptr+1)] = a;
}
static void PrcPUSHR(Processor* prc) {
    assert(prc);
    StackPush(prc->stk, prc->registers[(int)*(prc->current_command_ptr+1)]);
}
static void PrcPUSH(Processor* prc) {
    assert(prc);
    StackPush(prc->stk, *(prc->current_command_ptr+1));
}
static void PrcPOW(Processor* prc) {
    assert(prc);
    double a = 0;
    double b = 0;
    StackPop(prc->stk, &a);
    StackPop(prc->stk, &b);
    StackPush(prc->stk, pow(b, a));
}
static void PrcJMP(Processor* prc) {
    assert(prc);
    prc->current_command_ptr = prc->running_code_ptr + (int)*(prc->current_command_ptr + 1);
    getc(stdin);
}

static Actions DoCommand(Processor* prc, Actions action) {

    assert(prc);

    switch (action) {
        case Finish:
            PrcHLT(prc);                    break;
        case In:
            PrcIN(prc);                     break;
        case Out:
            PrcOUT(prc);                    break;
        case WriteValueToRegister:
            PrcPOPR(prc);                   break;
        case AddValueOfRegister:
            PrcPUSHR(prc);                  break;
        case AddingNumber:
            PrcPUSH(prc);                   break;
        case Addition:
            PRC_ARITHMETIC(prc, +);         break;
        case Substraction:
            PRC_ARITHMETIC(prc, -);         break;
        case Multiplication:
            PRC_ARITHMETIC(prc, *);         break;
        case Division:
            PRC_ARITHMETIC(prc, /);         break;
        case Powering:
            PrcPOW(prc);                    break;
        case Jumping:
            PrcJMP(prc);                    break;
        case JumpingIfBelow:
            PRC_CONDITION_JUMP(prc, >);     break;
        case JumpingIfBelowEquals:
            PRC_CONDITION_JUMP(prc, >=);    break;
        case JumpingIfAbove:
            PRC_CONDITION_JUMP(prc, <);     break;
        case JumpingIfAboveEquals:
            PRC_CONDITION_JUMP(prc, <=);    break;
        case JumpingIfEquals:
            PRC_CONDITION_JUMP(prc, ==);    break;
        case JumpingIfNotEquals:
            PRC_CONDITION_JUMP(prc, !=);    break;
        case None:
        default:
            break;
    }

    return action;

}

static stack_error_code DoCommands(Processor* prc, Actions* curr_act) {

    PRC_ASSERT_OK(prc);

    for (size_t index = 0; index < sizeof(commands)/sizeof(Command); index++) {

        if (commands[index].opcode == (int)*(prc->current_command_ptr)) {

            Actions curr_act = commands[index].action;

            return DoCommand(prc, curr_act);
        }

    }
    return Prc_Err_t;
}

stack_error_code void RunCode(Processor* prc) {

    PRC_ASSERT_OK(prc);

    Actions curr_act = None;
    while (curr_act) {
        DoCommands(prc, &curr_act);
        prc->current_command_ptr += commands[curr_act].offset;
    }

    PRC_ASSERT_OK(prc);

}


void PrcCtor(Processor* prc, const char* asm_file, BirthInfo* prc_info_got = nullptr) {

    assert(asm_file);

    #ifdef _DEBUG
    prc->init_info = prc_info_got;
    #endif //debug

    static StackInfo stack_for_processor = {};
    INIT_STACK(stack_for_processor, 10);
    prc->stk = &stack_for_processor;

    size_t filesize = 0;
    GetFileSize(asm_file, &filesize);

    FILE* file = fopen(asm_file, "r");

    int version = 0;

    fscanf(file, "%d", &version);
    if (version != VERSION) {
        fclose(file);
        return;
    }

    prc->current_command_ptr      = (double*)calloc(filesize, sizeof(double));
    prc->running_code_ptr  = prc->current_command_ptr;
    double val = 0;

    while (fscanf(file, "%lf", &val) == 1) {
        *(prc->running_code_ptr++) = val;
    }

    prc->running_code_ptr = prc->current_command_ptr;

    fclose(file);
}

void PrcDtor(Processor* prc) {
    assert(prc);

    free(prc->running_code_ptr);
    StkDtor(prc->stk);
}

void RunAssembler(Processor* prc) {

    assert(prc);
    RunCode(prc);
}

