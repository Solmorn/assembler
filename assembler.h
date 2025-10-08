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







Errors FillTextInfo(TextParams* text_info, const char* filename);
Errors CreateAndFillBuffer(TextParams* text_info, const char* filename, size_t filesize);
Errors AllocateText(TextParams* text_info);
Errors CreateAndFillText(TextParams* text_info);
Errors WriteResultInFile(FILE* result_f, TextParams* text_info);
Errors PrintLine(LineParams* str_params, FILE* result_file);
Errors Destruct(TextParams* text_info);
size_t CountBufferLines(char* buffer);


Actions CheckCommand(LineParams* line);
Actions FillAssemblerLine(LineParams* line, FILE* result);
Errors FillAssembler(TextParams* text_info, const char* result);



#endif
