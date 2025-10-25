#include "tools.h"

Errors GetFileSize(const char* filename, size_t* filesize) {

    assert(filename != nullptr);

    struct stat st;//off_t

    if (stat(filename, &st) == 0) {
        *filesize = st.st_size;
        return OkError;

    }

    return UnexpectedError;
}
