#ifndef TOOLS_H
#define TOOLS_H

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

static const int VERSION = 550500550;


enum Actions {
    Finish,
    Out,
    In,
    MemoryAddValueOfRegister,
    MemoryWriteValueToRegister,
    AddValueOfRegister,
    WriteValueToRegister,
    AddingNumber,
    Addition,
    Substraction,
    Multiplication,
    Division,
    Powering,
    Jumping,
    JumpingIfBelowEquals,
    JumpingIfBelow,
    JumpingIfAboveEquals,
    JumpingIfAbove,
    JumpingIfEquals,
    JumpingIfNotEquals,
    CallingPtr,
    Retting,
    Drawing,
    NoOp,

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
    int         opcode;
    size_t      length;
};

static const Command commands[]     =  {{"HLT",   0, 1, Finish,                      0,  3},
                                        {"OUT",   0, 1, Out,                         1,  3},
                                        {"IN",    0, 1, In,                          2,  2},
                                        {"PUSHM", 1, 2, MemoryAddValueOfRegister,    3,  5},//
                                        {"POPM",  1, 2, MemoryWriteValueToRegister,  4,  4},//
                                        {"PUSHR", 1, 2, AddValueOfRegister,          5,  5},//
                                        {"POPR",  1, 2, WriteValueToRegister,        6,  4},//
                                        {"PUSH",  1, 2, AddingNumber,                7,  4},
                                        {"ADD",   0, 1, Addition,                    8,  3},
                                        {"SUB",   0, 1, Substraction,                9,  3},
                                        {"MULT",  0, 1, Multiplication,              10, 4},
                                        {"DIV",   0, 1, Division,                    11, 3},
                                        {"POW",   0, 1, Powering,                    12, 3},
                                        {"JMP",   1, 0, Jumping,                     13, 3},
                                        {"JBE",   1, 0, JumpingIfBelowEquals,        14, 3},
                                        {"JB",    1, 0, JumpingIfBelow,              15, 2},
                                        {"JAE",   1, 0, JumpingIfAboveEquals,        16, 3},
                                        {"JA",    1, 0, JumpingIfAbove,              17, 2},
                                        {"JE",    1, 0, JumpingIfEquals,             18, 2},
                                        {"JNE",   1, 0, JumpingIfNotEquals,          19, 3},
                                        {"CALL",  1, 0, CallingPtr,                  20, 4},
                                        {"RET",   0, 0, Retting,                     21, 3},
                                        {"DRAW",  0, 1, Drawing,                     22, 4}};



struct LineParams {
    size_t len;
    char* str;
};

struct FileParams {
    size_t length;
    size_t number_of_strings;
    char* buffer;
    LineParams** file_lines;
    LineParams* all_lines_ptr;
    ssize_t metki[10] = {};
    double* asm_current_command_ptr = nullptr;
    double* asm_code_ptr = nullptr;
};

Errors GetFileSize(const char* filename, size_t* filesize);

#endif
