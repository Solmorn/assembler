#include "assembler.h"

Errors GetFileSize(const char* filename, size_t* filesize) {

    assert(filename != nullptr);

    struct stat st;//off_t

    if (stat(filename, &st) == 0) {
        *filesize = st.st_size;
        return OkError;

    }

    return UnexpectedError;
}

Errors FillTextInfo(TextParams* text_info, const char* filename, size_t filesize) {

    assert(filename  != nullptr);
    assert(text_info != nullptr);

    CreateAndFillBuffer(text_info, filename, filesize);

    text_info->number_of_strings = CountBufferLines(text_info->buffer);
    CreateAndFillText(text_info);

    return OkError;
}

Errors CreateAndFillBuffer(TextParams* text_info, const char* filename, size_t filesize) {

    assert(filename  != nullptr);
    assert(text_info != nullptr);


    int descriptor = open(filename, O_RDONLY);

    if (descriptor == -1) {
        return UnexpectedError;
    }

    text_info->buffer = (char*)calloc(filesize+2, sizeof(char));

    if (text_info->buffer == nullptr) {
        return UnexpectedError;
    }

    ssize_t signed_length = read(descriptor, text_info->buffer, filesize+2);

    if (signed_length == -1) {
        return UnexpectedError;
    }

    (text_info->buffer)[signed_length++] = '\n';
    (text_info->buffer)[signed_length]   = '\0';

    text_info->length = (size_t)signed_length + 2;
    text_info->number_of_strings++;

    close(descriptor);

    return OkError;
}

Errors AllocateText(TextParams* text_info) {

    assert(text_info != nullptr);

    text_info->text = (LineParams**)calloc(text_info->number_of_strings, sizeof(LineParams*));

    if (text_info->text == nullptr) {
        return UnexpectedError;
    }

    text_info->all_lines_ptr = (LineParams*)calloc(text_info->number_of_strings, sizeof(LineParams));

    if (text_info->all_lines_ptr == nullptr) {
        return UnexpectedError;
    }

    for (size_t i = 0; i < text_info->number_of_strings; i++) {
        (text_info->text)[i] = text_info->all_lines_ptr + i;
    }

    return OkError;
}

Errors CreateAndFillText(TextParams* text_info) {

    assert(text_info != nullptr);

    Errors err = AllocateText(text_info);

    if (err != OkError) {
        return err;
    }

    size_t strings_added = 0;

    LineParams** text_ptr        = text_info->text;
    char* prev_pointer_to_string = text_info->buffer;
    char* destination_pointer    = text_info->buffer + text_info->length-1;

    for (char* pointer = text_info->buffer; pointer < destination_pointer; pointer++) {

        if (*pointer == '\n') {

            text_ptr[strings_added]->str = prev_pointer_to_string;
            text_ptr[strings_added]->len = (size_t)(pointer + 1 - prev_pointer_to_string);

            strings_added++;
            prev_pointer_to_string = pointer + 1; //offset caused by pointer's position on '\n'(L3 английский имеет свой эффект)

        }
    }

    return OkError;
}

Errors WriteResultInFile(FILE* result_file, TextParams* text_info) {

    assert(result_file != nullptr);
    assert(text_info   != nullptr);

    size_t text_strings = text_info->number_of_strings;

    for (size_t i = 0; i < text_strings; i++) {
        PrintLine((text_info->text)[i], result_file);
    }

    return OkError;
}

Errors PrintLine(LineParams* str_params, FILE* result_file) {

    assert(result_file != nullptr);
    assert(str_params  != nullptr);

    size_t string_length = str_params->len;

    for (size_t j = 0; j < string_length; j++){
        fputc((str_params->str)[j], result_file);
    }

    return OkError;
}

Errors Destruct(TextParams* text_info) {

    assert(text_info != nullptr);

    free(text_info->buffer);
    free(text_info->text);
    free(text_info->all_lines_ptr);

    return OkError;
}

size_t CountBufferLines(char* buffer) {

    assert(buffer != nullptr);

    size_t count_lines = 0;

    while (*buffer++) {
        if (*buffer == '\n') {
            count_lines++;
        }
    }

    return count_lines;
}


Actions CheckCommand(LineParams* line) {

    assert(line);

    for (size_t index = 0; index < sizeof(commands)/sizeof(Command); index++) {
        if (!strncmp(commands[index].name, line->str, commands[index].length)) {

            return commands[index].action;
        }
    }
    return None;
}

Actions FillAssemblerLine(LineParams* line, FILE* result) {

    assert(line);
    assert(result);

    Actions action = CheckCommand(line);

    if (action != None) {

        char* line_cpy_ptr = line->str;
        size_t index = 0;

        if (action != AddValueOfRegister && action != WriteValueToRegister && commands[action].args == 1) {

            while (line_cpy_ptr[index] != ' ') index++;

            fprintf(result, "%d", commands[action].enumeration);

            while (index < line->len){
                fputc(line_cpy_ptr[index], result);
                index++;
            }
        } else if (action == AddValueOfRegister || action == WriteValueToRegister) {

            while (line_cpy_ptr[index] != ' ') index++;

            fprintf(result, "%d ", commands[action].enumeration);

            fprintf(result, "%d\n", line_cpy_ptr[index+2] - 'A');


        } else {
            fprintf(result, "%d\n", commands[action].enumeration);
        }

    }

    return action;

}

void FillAssembler(TextParams* text_info, const char* result) {

    assert(text_info);
    assert(result);
    FILE* result_f = fopen(result, "w");
    for (size_t index = 0; index < text_info->number_of_strings; index++) {
        LineParams* line = text_info->text[index];

        if (FillAssemblerLine(line, result_f) == Finish) break;

    }
    fclose(result_f);
}
