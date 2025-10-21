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

    double* for_asm_code_ptr = (double*)calloc(text_info->number_of_strings * 2, sizeof(double));
    if (for_asm_code_ptr == nullptr) return UnexpectedError;

    text_info->asm_code_ptr            = for_asm_code_ptr;
    text_info->asm_current_command_ptr = for_asm_code_ptr;

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

static Actions FillAssemblerCode(FileParams* text_info, LineParams* line) {

    assert(line);

    if (*(line->str) == ':') {
        text_info->metki[*(line->str+1)-'0'] = text_info->asm_current_command_ptr - text_info->asm_code_ptr;
        return NoOp;
    }

    Actions action = CheckCommand(line);

    if (action != None) {

        char* line_cpy_ptr = line->str;
        size_t index = 0;


        if (commands[action].args == 1) {

            while (line_cpy_ptr[index] != ' ') index++;

            *(text_info->asm_current_command_ptr) = commands[action].opcode;
            text_info->asm_current_command_ptr++;

            char curr_char = line_cpy_ptr[index+1];

            if ((curr_char >= '0' && curr_char <= '9') || curr_char == '-') {
                char* end_ptr = line_cpy_ptr+index+1 + line->len - 1;
                *(text_info->asm_current_command_ptr) = strtod(line_cpy_ptr+index+1, &end_ptr); //
                text_info->asm_current_command_ptr++;

            } else if (curr_char == ':') {

                *(text_info->asm_current_command_ptr) = (double)text_info->metki[line_cpy_ptr[index+2] - '0'];
                text_info->asm_current_command_ptr++;

            } else if (curr_char == '['){

                *(text_info->asm_current_command_ptr) = line_cpy_ptr[index+3] - 'A';
                text_info->asm_current_command_ptr++;

            } else {
                *(text_info->asm_current_command_ptr) = line_cpy_ptr[index+2] - 'A';
                text_info->asm_current_command_ptr++;
            }

        } else {
            *(text_info->asm_current_command_ptr) = commands[action].opcode;
            text_info->asm_current_command_ptr++;
        }

    }

    return action;

}

static Actions FillAssemblerLine(FileParams* text_info, FILE* result) {

    assert(result);

    Actions action = (Actions)*(text_info->asm_current_command_ptr);

    if (action != None) {

        if (commands[action].args == 1) {

            fprintf(result, "%d ", commands[action].opcode);
            text_info->asm_current_command_ptr++;

            fprintf(result, "%lf\n", (double)*text_info->asm_current_command_ptr);
            text_info->asm_current_command_ptr++;

        } else {
            fprintf(result, "%d\n", commands[action].opcode);
            text_info->asm_current_command_ptr++;
        }
        return action;
    }

    return None;

}

Errors FillAssemblerFile(FileParams* text_info, const char* result, const char* commands_file) {

    assert(text_info);
    assert(result);


    if (FillFileInfo(text_info, commands_file) == UnexpectedError) return UnexpectedError;

    for (size_t index = 0; index < text_info->number_of_strings; index++) {
        LineParams* line = text_info->file_lines[index];
        Actions action = None;
        if ((action = FillAssemblerCode(text_info, line)) == None) break;
    }
    text_info->asm_current_command_ptr = text_info->asm_code_ptr;
    for (size_t index = 0; index < text_info->number_of_strings; index++) {
        LineParams* line = text_info->file_lines[index];
        if (FillAssemblerCode(text_info, line) == None) break;
    }
    text_info->asm_current_command_ptr = text_info->asm_code_ptr;

    FILE* result_f = fopen(result, "w");
    if (result_f == nullptr) return UnexpectedError;

    fprintf(result_f, "%d\n\n", VERSION);

    size_t commans_counter = 0;
    while (FillAssemblerLine(text_info, result_f) != None && commans_counter++ < text_info->number_of_strings * 2);

    fclose(result_f);
    DestructFileParams(text_info);
    return OkError;
}
