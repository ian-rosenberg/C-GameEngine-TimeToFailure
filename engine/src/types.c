#include "types.h"

const char* extract_filename_string(const char* path) {
    const char* lastSlash = strrchr(path, '/');
    const char* result = NULL;
    char* context = NULL;

    if (lastSlash == NULL) {
        lastSlash = strrchr(path, '\\');
        result = lastSlash ? strtok_s((char*)lastSlash, ".", &context) : lastSlash;
    }
    if (lastSlash == NULL) {
        result = lastSlash ? strtok_s((char*)lastSlash, ".", &context) : lastSlash;
        return result;
    }

    return result;
}

const char* copy_const_char(const char* source) {
    if (source == NULL) {
        return NULL;
    }

    size_t length = strlen(source) + 1;
    char* copy = malloc(length);
    if (copy != NULL) {
        strcpy_s(copy, length, source);
    }
    return copy;
}