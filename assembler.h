#ifndef ASSEMBLER_H
#define ASSEMBLER_H


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


Errors FillAssemblerFile(FileParams* text_info, const char* result, const char* commands_file);


#endif

