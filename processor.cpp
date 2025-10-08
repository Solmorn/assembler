#include "processor.h"


void PrcHLT(Processor* prc) {
    assert(prc);
    printf("PROGRAM_ENDED_OK\n\n");
}
void PrcOUT(Processor* prc) {
    assert(prc);
    double a = 0;
    StackPop(prc->stk, &a);
    printf("%lf\n", a);
}
void PrcIN(Processor* prc) {
    assert(prc);
    double a = 0;
    scanf("lf", &a);
    StackPush(prc->stk, a);
}
void PrcPOPR(Processor* prc) {
    assert(prc);
    double a = 0;
    StackPop(prc->stk, &a);
    prc->registers[(int)*(prc->code+1)] = a;
}
void PrcPUSHR(Processor* prc) {
    assert(prc);
    StackPush(prc->stk, prc->registers[(int)*(prc->code+1)]);
}
void PrcPUSH(Processor* prc) {
    assert(prc);
    StackPush(prc->stk, *(prc->code+1));
}
void PrcADD(Processor* prc) {
    assert(prc);
    double a = 0;
    double b = 0;
    StackPop(prc->stk, &a);
    StackPop(prc->stk, &b);
    StackPush(prc->stk, b+a);
}
void PrcSUB(Processor* prc) {
    assert(prc);
    double a = 0;
    double b = 0;
    StackPop(prc->stk, &a);
    StackPop(prc->stk, &b);
    StackPush(prc->stk, b-a);
}
void PrcMULT(Processor* prc) {
    assert(prc);
    double a = 0;
    double b = 0;
    StackPop(prc->stk, &a);
    StackPop(prc->stk, &b);
    StackPush(prc->stk, b*a);
}
void PrcDIV(Processor* prc) {
    assert(prc);
    double a = 0;
    double b = 0;
    StackPop(prc->stk, &a);
    StackPop(prc->stk, &b);
    StackPush(prc->stk, b/a);
}
void PrcPOW(Processor* prc) {
    assert(prc);
    double a = 0;
    double b = 0;
    StackPop(prc->stk, &a);
    StackPop(prc->stk, &b);
    StackPush(prc->stk, pow(b, a));
}
void PrcJMP(Processor* prc) {
    assert(prc);
    prc->code = prc->code_cpy + (int)*(prc->code + 1);
    getc(stdin);
}
void PrcJBE(Processor* prc) {
    assert(prc);
    double a = 0;
    double b = 0;
    StackPop(prc->stk, &a);
    StackPop(prc->stk, &b);
    if (a >= b) prc->code = prc->code_cpy + (int)*(prc->code + 1);
    else prc->code += 2; //bad
}
void PrcJB(Processor* prc) {
    assert(prc);
    double a = 0;
    double b = 0;
    StackPop(prc->stk, &a);
    StackPop(prc->stk, &b);
    if (a > b) prc->code = prc->code_cpy + (int)*(prc->code + 1);
    else prc->code += 2; //bad
}
void PrcJAE(Processor* prc) {
    assert(prc);
    double a = 0;
    double b = 0;
    StackPop(prc->stk, &a);
    StackPop(prc->stk, &b);
    if (a <= b) prc->code = prc->code_cpy + (int)*(prc->code + 1);
    else prc->code += 2; //bad
}
void PrcJA(Processor* prc) {
    assert(prc);
    double a = 0;
    double b = 0;
    StackPop(prc->stk, &a);
    StackPop(prc->stk, &b);
    if (a < b) prc->code = prc->code_cpy + (int)*(prc->code + 1);
    else prc->code += 2; //bad
}
void PrcJE(Processor* prc) {
    assert(prc);
    double a = 0;
    double b = 0;
    StackPop(prc->stk, &a);
    StackPop(prc->stk, &b);
    if (a == b) prc->code = prc->code_cpy + (int)*(prc->code + 1);
    else prc->code += 2; //bad
}
void PrcJNE(Processor* prc) {
    assert(prc);
    double a = 0;
    double b = 0;
    StackPop(prc->stk, &a);
    StackPop(prc->stk, &b);
    if (a != b) prc->code = prc->code_cpy + (int)*(prc->code + 1);
    else prc->code += 2; //bad
}


Actions DoCommand(Processor* prc, Actions action) {

    assert(prc);

    switch (action) {
        case Finish:
            PrcHLT(prc);   break;
        case In:
            PrcIN(prc);    break;
        case Out:
            PrcOUT(prc);   break;
        case WriteValueToRegister:
            PrcPOPR(prc);  break;
        case AddValueOfRegister:
            PrcPUSHR(prc); break;
        case AddingNumber:
            PrcPUSH(prc);  break;
        case Addition:
            PrcADD(prc);   break;
        case Substraction:
            PrcSUB(prc);   break;
        case Multiplication:
            PrcMULT(prc);  break;
        case Division:
            PrcDIV(prc);   break;
        case Powering:
            PrcPOW(prc);   break;
        case Jumping:
            PrcJMP(prc);   break;
        case JumpingIfBelow:
            PrcJB(prc);    break;
        case JumpingIfBelowEquals:
            PrcJBE(prc);   break;
        case JumpingIfAbove:
            PrcJA(prc);    break;
        case JumpingIfAboveEquals:
            PrcJAE(prc);   break;
        case JumpingIfEquals:
            PrcJE(prc);    break;
        case JumpingIfNotEquals:
            PrcJNE(prc);   break;
        default:
            break;
    }

    return action;

}

Actions DoCommands(Processor* prc) {

    assert(prc);

    for (size_t index = 0; index < sizeof(commands)/sizeof(Command); index++) {

        if (commands[index].opcode == (int)*(prc->code)) {

            Actions curr_act = commands[index].action;

            return DoCommand(prc, curr_act);
        }

    }
    return None;
}

void RunCode(Processor* prc) {

    assert(prc);

    Actions curr_act = None;
    while (curr_act = DoCommands(prc)) {
        prc->code += commands[curr_act].offset;
    }


    StkDtor(prc->stk);

}

void ProcessorCtor(Processor* prc, const char* asm_file) {

    assert(asm_file);

    size_t filesize = 0;
    GetFileSize(asm_file, &filesize);

    FILE* file = fopen(asm_file, "r");

    int version = 0;

    fscanf(file, "%d", &version);
    if (version != VERSION) {
        fclose(file);
        return;
    }

    prc->code      = (double*)calloc(filesize, sizeof(double));
    prc->registers = (double*)calloc(10,       sizeof(double));

    prc->code_cpy = prc->code;
    double val = 0;

    while (fscanf(file, "%lf", &val) == 1) {
        *(prc->code_cpy++) = val;
    }

    prc->code_cpy = prc->code;


    fclose(file);


}


void RunAssembler(Processor* prc, const char* asm_file) {

    assert(asm_file);

    ProcessorCtor(prc, asm_file);

    RunCode(prc);

    //Processor dtor

}
