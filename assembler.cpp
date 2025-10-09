#include "assembler.h"


static Errors FillFileInfo(FileParams* text_info, const char* filename);
static Errors CreateAndFillBuffer(FileParams* text_info, const char* filename, size_t filesize);
static Errors AllocateFileLinesPtr(FileParams* text_info);
static Errors CreateAndFillFileLinesPtr(FileParams* text_info);
static Errors DestructFileParams(FileParams* text_info);
static size_t CountBufferLines(char* buffer);

static Errors FillFileInfo(FileParams* text_info, const char* filename) {

    assert(filename  != nullptr);
    assert(text_info != nullptr);

    size_t filesize = 0;
    if (GetFileSize(filename, &filesize)                   == UnexpectedError) return UnexpectedError;

    if (CreateAndFillBuffer(text_info, filename, filesize) == UnexpectedError) return UnexpectedError;

    text_info->number_of_strings = CountBufferLines(text_info->buffer);

    if (CreateAndFillFileLinesPtr(text_info)                       == UnexpectedError) return UnexpectedError;

    return OkError;
}

static Errors CreateAndFillBuffer(FileParams* text_info, const char* filename, size_t filesize) {

    assert(filename  != nullptr);
    assert(text_info != nullptr);


    int descriptor = open(filename, O_RDONLY);

    if (descriptor == -1) {
        return UnexpectedError;
    }

    char* for_buffer = (char*)calloc(filesize+2, sizeof(char));

    if (for_buffer == nullptr) {
        return UnexpectedError;
    }

    text_info->buffer = for_buffer;

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

static Errors AllocateFileLinesPtr(FileParams* text_info) {

    assert(text_info != nullptr);

    LineParams** for_file_lines = (LineParams**)calloc(text_info->number_of_strings, sizeof(LineParams*));

    if (for_file_lines == nullptr) {

        return UnexpectedError;
    }
    text_info->file_lines = for_file_lines;


    LineParams* for_all_lines_ptr = (LineParams*)calloc(text_info->number_of_strings, sizeof(LineParams));

    if (for_all_lines_ptr == nullptr) {

        return UnexpectedError;
    }
    text_info->all_lines_ptr = for_all_lines_ptr;

    for (size_t i = 0; i < text_info->number_of_strings; i++) {
        (text_info->file_lines)[i] = text_info->all_lines_ptr + i;
    }

    return OkError;
}

static Errors CreateAndFillFileLinesPtr(FileParams* text_info) {

    assert(text_info != nullptr);

    Errors err = AllocateFileLinesPtr(text_info);

    if (err != OkError) {
        return err;
    }

    size_t strings_added = 0;

    LineParams** text_ptr        = text_info->file_lines;
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

static Errors DestructFileParams(FileParams* text_info) {

    assert(text_info != nullptr);

    free(text_info->buffer);
    free(text_info->file_lines);
    free(text_info->all_lines_ptr);

    return OkError;
}

static size_t CountBufferLines(char* buffer) {

    assert(buffer != nullptr);

    size_t count_lines = 0;

    while (*buffer++) {
        if (*buffer == '\n') {
            count_lines++;
        }
    }

    return count_lines;
}

static Actions CheckCommand(LineParams* line) {

    assert(line);

    for (size_t index = 0; index < sizeof(commands)/sizeof(Command); index++) {
        if (!strncmp(commands[index].name, line->str, commands[index].length)) {

            return commands[index].action;
        }
    }
    return None;
}

static Actions FillAssemblerLine(LineParams* line, FILE* result) {

    assert(line);
    assert(result);

    Actions action = CheckCommand(line);

    if (action != None) {

        char* line_cpy_ptr = line->str;
        size_t index = 0;

        if (commands[action].args == 1) {

            while (line_cpy_ptr[index] != ' ') index++;

            fprintf(result, "%d", commands[action].opcode);

            char curr_char = line_cpy_ptr[index+1];

            if (curr_char >= '0' && curr_char <= '9') {

                while (index < line->len){

                    fputc(line_cpy_ptr[index], result);
                    index++;

                }
            } else {

                fprintf(result, " %d\n", line_cpy_ptr[index+2] - 'A');
            }

        } else {
            fprintf(result, "%d\n", commands[action].opcode);
        }

    }

    return action;

}

Errors FillAssemblerFile(FileParams* text_info, const char* result, const char* commands_file) {

    assert(text_info);
    assert(result);

    if (FillFileInfo(text_info, commands_file) == UnexpectedError) return UnexpectedError;

    FILE* result_f = fopen(result, "w");
    if (result_f == nullptr) return UnexpectedError;

    fprintf(result_f, "%d\n\n", VERSION);
    for (size_t index = 0; index < text_info->number_of_strings; index++) {
        LineParams* line = text_info->file_lines[index];

        if (FillAssemblerLine(line, result_f) == Finish) break;

    }
    fclose(result_f);
    DestructFileParams(text_info);
    return OkError;
}
