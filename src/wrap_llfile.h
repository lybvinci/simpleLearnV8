#ifndef WRAP_LLFILE_H_
#define WRAP_LLFILE_H_

#include <include/v8.h>
#include "llfile.h"

namespace ll {
void V8ReadFile(const v8::FunctionCallbackInfo<v8::Value>& args);
void V8GetFileSize(const v8::FunctionCallbackInfo<v8::Value>& args);
void V8WriteFile(const v8::FunctionCallbackInfo<v8::Value>& args);
} // namespace ll

#endif // WRAP_LLFILE_H_
