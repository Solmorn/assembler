#ifndef TOOLS_H
#define TOOLS_H

static const int version = 550500550;

enum Actions {
    Finish,
    Out,
    In,
    AddValueOfRegister,
    WriteValueToRegister,
    AddingNumber,
    Addition,
    Substraction,
    Multiplication,
    Division,
    Powering,
    Jumping,
    JumpingIfBelow,
    JumpingIfBelowEquals,
    JumpingIfAbove,
    JumpingIfAboveEquals,
    JumpingIfEquals,
    JumpingIfNotEquals,

    None
};

enum Errors {
    UnexpectedError,
    OkError
};

struct Command {
    const char* name;
    size_t      args;
    size_t      offset;
    Actions     action;
    size_t      enumeration;
    size_t      length;
};

static Command commands[]     =        {{"HLT",   0, 1, Finish,                0,  3},
                                        {"OUT",   0, 1, Out,                   1,  3},
                                        {"IN",    0, 1, In,                    2,  2},
                                        {"PUSHR", 1, 2, AddValueOfRegister,    9,  5},//
                                        {"POPR",  1, 2, WriteValueToRegister,  10, 4},//
                                        {"PUSH",  1, 2, AddingNumber,          3,  4},
                                        {"ADD",   0, 1, Addition,              4,  3},
                                        {"SUB",   0, 1, Substraction,          5,  3},
                                        {"MULT",  0, 1, Multiplication,        6,  4},
                                        {"DIV",   0, 1, Division,              7,  3},
                                        {"POW",   0, 1, Powering,              8,  3},
                                        {"JMP",   1, 0, Jumping,               11, 3},
                                        {"JB",    1, 0, JumpingIfBelow,        12, 2}};



struct LineParams {
    size_t len;
    char* str;
};

struct TextParams {
    size_t length;
    size_t number_of_strings;
    char* buffer;
    LineParams** text;
    LineParams* all_lines_ptr;
};



#endif
