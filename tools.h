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

static const Command commands[]     =  {{"HLT",   0, 1, Finish,                0,  3},
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
                                        {"JBE",   1, 0, JumpingIfBelowEquals,  12, 3},
                                        {"JB",    1, 0, JumpingIfBelow,        13, 2},
                                        {"JAE",   1, 0, JumpingIfAboveEquals,  14, 3},
                                        {"JA",    1, 0, JumpingIfAbove,        15, 2},
                                        {"JE",    1, 0, JumpingIfEquals,       16, 2},
                                        {"JNE",   1, 0, JumpingIfNotEquals,    17, 3}};



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
};

Errors GetFileSize(const char* filename, size_t* filesize);

#endif
