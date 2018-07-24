#include "llfile.h"
#include <stdio.h>
#include <string.h>

namespace ll {

size_t file_size(const char* file_path) {
    FILE* file = fopen(file_path, "rb");
    if (file == NULL) {
        return 0;
    }
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fclose(file);
    return size;
}

LLFile* read_file(const char* file_path) {
    FILE* file = fopen(file_path, "rb");
    LLFile result;
    if (file == NULL) {
        return &result;
    }
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    char* chars = new char[size + 1];
    chars[size] = '\0';
    for (size_t i = 0; i < size;) {
        i += fread(&chars[i], 1, size - i, file);
        if (ferror(file)) {
            fclose(file);
            return &result;
        }
    }
    fclose(file);
    result.result = chars;
    result.size = size;
    return &result;
}

int write_file(const char* file_path, const char* res) {
    if (res == NULL) {
        return -1;
    }
    FILE* file = fopen(file_path, "w");
    fwrite(res, sizeof(char), strlen(res), file);
    fclose(file);
    return 0;
}

} // namespace ll