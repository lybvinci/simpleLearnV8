#ifndef LLFILE_H_
#define LLFILE_H_

#include <stdlib.h>

namespace ll {
typedef struct {
    char* result;
    size_t size;
} LLFile;
LLFile* read_file(const char* file_path);
int write_file(const char* file_path, const char* res);
size_t file_size(const char* file_name);
} // namespace ll


#endif // LLFILE_H_INCLUDE