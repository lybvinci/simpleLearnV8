#include "wrap_llfile.h"
#include "llfile.h"

namespace ll {
void V8ReadFile(const v8::FunctionCallbackInfo<v8::Value>& args) {
    if (args.Length != 1) {
        args.GetIsolate()->ThrowException(
            v8::String::NewFromUtf8(args.GetIsolate(), "bad parameter.",
            v8::NewStringType::kNormal).ToLocalChecked());
        return;
    }
    v8::String::Utf8Value file(args.GetIsolate(), args[0]);
    if (*file == NULL) {
        args.GetIsolate()->ThrowException(
            v8::String::NewFromUtf8(args.GetIsolate(), "Error loading file.",
            v8::NewStringType::kNormal).ToLocalChecked());
        return;
    }
    v8::Local<v8::String> source;
    if (!ReadFile(args.GetIsolate(), *file).ToLocal(&source)) {
        args.GetIsolate()->ThrowException(
            v8::String::NewFromUtf8(args.GetIsolate(), "Error read file.",
            v8::NewStringType::kNormal).ToLocalChecked());
        return;
    }
    
    args.GetReturnValue().Set(source);
}

void V8GetFileSize(const v8::FunctionCallbackInfo<v8::Value>& args) {
    if (args.Length != 1) {
        args.GetIsolate()->ThrowException(
            v8::String::NewFromUtf8(args.GetIsolate(), "bad parameter.",
            v8::NewStringType::kNormal).ToLocalChecked());
        return;
    }
    v8::String::Utf8Value file(args.GetIsolate(), args[0]);
    if (*file == NULL) {
        args.GetIsolate()->ThrowException(
            v8::String::NewFromUtf8(args.GetIsolate(), "Error loading file.",
            v8::NewStringType::kNormal).ToLocalChecked());
        return;
    }
    size_t size = file_size(*file);
    int result = static_cast<int>(size);
    args.GetReturnValue().Set(result);
}

void V8WriteFile(const v8::FunctionCallbackInfo<v8::Value>& args) {
    if (args.Length != 2) {
        args.GetIsolate()->ThrowException(
            v8::String::NewFromUtf8(args.GetIsolate(), "bad parameter.",
            v8::NewStringType::kNormal).ToLocalChecked());
        return;
    }
    v8::String::Utf8Value file(args.GetIsolate(), args[0]);
    v8::String::Utf8Value text(args.GetIsolate(), args[1]);
    if (*file == NULL || *text == NULL) {
        args.GetIsolate()->ThrowException(
            v8::String::NewFromUtf8(args.GetIsolate(), "Error read file",
            v8::NewStringType::kNormal).ToLocalChecked());
        return;
    }
    int ret = write_file(*file, *text);
    args.GetReturnValue().Set(ret);
}

v8::MaybeLocal<v8::String> ReadFile(v8::Isolate* isolate, const char* name) {
    LLFile* res_file = read_file(name);
    if (res_file->result == NULL) {
        return v8::MaybeLocal<v8::String>();
    }
    v8::MaybeLocal<v8::String> result = v8::String::NewFromUtf8(isolate, 
        res_file->result,
        v8::NewStringType::kNormal,
        res_file->size);
    delete[] res_file->result;
    return result;
}
} //namespace ll