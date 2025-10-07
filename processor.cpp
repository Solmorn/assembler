#include "processor.h"


Errors asmGetFileSize(const char* filename, size_t* filesize) {

    assert(filename != nullptr);

    struct stat st;//off_t

    if (stat(filename, &st) == 0) {
        *filesize = st.st_size;
        return OkError;

    }

    return UnexpectedError;
}


Actions DoCommand(Processor* prc, Actions action) {


    assert(prc);

    double a = 0;
    double b = 0;

    switch (action) {
        case Finish:
            break;
        case WriteValueToRegister:
            StackPop(prc->stk, &a);
            prc->registers[(int)*(prc->code+1)] = a;
            break;
        case AddValueOfRegister:
            StackPush(prc->stk, prc->registers[(int)*(prc->code+1)]);
            break;
        case Out:
            StackPop(prc->stk, &a);
            printf("%lf\n", a);
            break;
        case AddingNumber:
            StackPush(prc->stk, *(prc->code+1));
            break;
        case Addition:
            StackPop(prc->stk, &a);
            StackPop(prc->stk, &b);
            StackPush(prc->stk, b+a);
            break;
        case Substraction:
            StackPop(prc->stk, &a);
            StackPop(prc->stk, &b);
            StackPush(prc->stk, b-a);
            break;
        case Multiplication:
            StackPop(prc->stk, &a);
            StackPop(prc->stk, &b);
            StackPush(prc->stk, b*a);
            break;
        case Division:
            StackPop(prc->stk, &a);
            StackPop(prc->stk, &b);
            StackPush(prc->stk, b/a);
            break;
        case Powering:
            StackPop(prc->stk, &a);
            StackPop(prc->stk, &b);
            StackPush(prc->stk, pow(b, a));
            break;
        case Jumping:
            prc->code = prc->code_cpy + (int)*(prc->code + 1);
            getc(stdin);
            break;
        case JumpingIfBelow:
            StackPop(prc->stk, &a);
            StackPop(prc->stk, &b);
            if (a > b) prc->code = prc->code_cpy + (int)*(prc->code + 1);
            else prc->code += 2; //bad
            break;
        case JumpingIfBelowEquals:
            StackPop(prc->stk, &a);
            StackPop(prc->stk, &b);
            if (a >= b) prc->code = prc->code_cpy + (int)*(prc->code + 1);
            else prc->code += 2; //bad
            break;
        case JumpingIfAbove:
            StackPop(prc->stk, &a);
            StackPop(prc->stk, &b);
            if (a < b) prc->code = prc->code_cpy + (int)*(prc->code + 1);
            else prc->code += 2; //bad
            break;
        case JumpingIfAboveEquals:
            StackPop(prc->stk, &a);
            StackPop(prc->stk, &b);
            if (a <= b) prc->code = prc->code_cpy + (int)*(prc->code + 1);
            else prc->code += 2; //bad
            break;
        case JumpingIfEquals:
            StackPop(prc->stk, &a);
            StackPop(prc->stk, &b);
            if (a == b) prc->code = prc->code_cpy + (int)*(prc->code + 1);
            else prc->code += 2; //bad
            break;
        case JumpingIfNotEquals:
            StackPop(prc->stk, &a);
            StackPop(prc->stk, &b);
            if (a != b) prc->code = prc->code_cpy + (int)*(prc->code + 1);
            else prc->code += 2; //bad
            break;
        default:
            break;
    }


    return action;
}

Actions DoCommands(Processor* prc) {

    assert(prc);

    for (size_t index = 0; index < sizeof(commands)/sizeof(Command); index++) {

        if (commands[index].enumeration == (int)*(prc->code)) {

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
    asmGetFileSize(asm_file, &filesize);

    FILE* file = fopen(asm_file, "r");

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
