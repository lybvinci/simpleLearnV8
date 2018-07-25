#include <include/v8.h>
#include <include/libplatform/libplatform.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <string.h>

namespace ll{
void Version(const v8::FunctionCallbackInfo<v8::Value>& args);
static bool run_shell;
static int main(int argc, char* argv[]) {
    v8::V8::InitializeICUDefaultLocation(argv[0]);
    v8::V8::InitializeExternalStartupData(argv[0]);
    std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
    v8::V8::InitializePlatform(platform.get());
    v8::V8::Initialize();
    v8::V8::SetFlagsFromCommandLine(&argc, argv, true);
    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    v8::Isolate* isolate = v8::Isolate::New(create_params);
    int result;
    {
        v8::Isolate::Scope isolate_scope(isolate);
        v8::HandleScope handle_scope(isolate);
        v8::Local<v8::Context> context = CreateShellContext(isolate);
        if (context.IsEmpty()) {
            fprintf(stderr, "Error creating context\n");
            return 1;
        }
        v8::Context::Scope context_scope(context);
        result = RunMain(isolate, platform.get(), argc, argv);
        if (run_shell) RunShell(context, platform.get());
    }
    isolate->Dispose();
    v8::V8::Dispose();
    v8::V8::ShutdownPlatform();
    delete create_params.array_buffer_allocator;
    return result;
}

v8::Local<v8::Context> CreateShellContext(v8::Isolate* isolate) {
    v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
    global->Set(
        v8::String::NewFromUtf8(isolate, "version", v8::NewStringType::kNormal).ToLocalChecked(),
        v8::FunctionTemplate::New(isolate, Version));
    return v8::Context::New(isolate, NULL, global);
}

int RunMain(v8::Isolate* isolate, v8::Platform* platform, int argc, char* argv[]) {
    for (int i = 1; i < argc ; i++) {
        const char* str = argv[i];
        if (strcmp(str, "-f") == 0) {
            continue;
        } else {
            // TODO  implement read file.
            return 0;
        }
    }
    return 0;
}

int RUnShell(v8::Local<v8::Context> context, v8::Platform* platform) {
    fprintf(stderr, "V8 version %s [shell]\n", v8::V8::GetVersion());
    static const int kBufferSize = 256;
    // Enter the execution enviroment before evaluating any code.
    v8::Context::Scope context_scope(context);
    v8::Local<v8::String> name(
        v8::String::NewFromUtf8(context->GetIsolate(), "(shell)",
        v8::NewStringType::kNormal).ToLocalChecked());
    while (true) {
        char buffer[kBufferSize];
        fprintf(stderr, "> ");
        char* str = fgets(buffer, kBufferSize, stdin);
        if (str == NULL) break;
        v8::HandleScope handle_scope(context->GetIsolate());
        ExecuteString(
            context->GetIsolate(),
            v8::String::NewFromUtf8(context->GetIsolate(), str,
            v8::NewStringType::kNormal).ToLocalChecked(),
            name, true, true);
        while (v8::platform::PumpMessageLoop(platform, context->GetIsolate()))
        continue;
    }
    fprintf(stderr, "\n");
}

bool ExecuteString(v8::Isolate* isolate, v8::Local<v8::String> source, v8::Local<v8::Value> name,bool print_result, bool report_exceptions) {
    v8::HandleScope handle_scope(isolate);
    v8::TryCatch try_catch(isolate);
    v8::ScriptOrigin origin(name);
    v8::Local<v8::Context> context(isolate->GetCurrentContext());
    v8::Local<v8::Script> script;
    if (!v8::Script::Compile(context, source, &origin).ToLocal(&script)) {
        if (report_exceptions) {
            ReportException(isolate, &try_catch);
        }
        return false;
    } else {
        v8::Local<v8::Value> result;
        if (!script->Run(context).ToLocal(&result)) {
            assert(try_catch.HasCaught());
            if (report_exceptions) {
                ReportException(isolate, &try_catch);
            }
            return false;
        } else {
            assert(!try_catch.HasCaught());
            if (print_result && !result->IsUndefined()) {
                v8::String::Utf8Value str(isolate, result);
                const char* cstr = ToCString(str);
                printf("%s\n", cstr);
            }
            return true;
        }
    }
}

const char* ToCString(const v8::String::Utf8Value& value) {
    return *value ? *value : "<string conversion failed>";
}

void ReportException(v8::Isolate* isolate, v8::TryCatch* try_catch) {
    v8::HandleScope handle_scope(isolate);
    v8::String::Utf8Value exception(isolate, try_catch->Exception());
    const char* exception_string = ToCString(exception);
    v8::Local<v8::Message> message = try_catch->Message();
    if (message.IsEmpty()) {
        fprintf(stderr, "%s\n", exception_string);
    } else {
        v8::String::Utf8Value filename(isolate, message->GetScriptOrigin().ResourceName());
        v8::Local<v8::Context> context(isolate->GetCurrentContext());
        const char* filename_string = ToCString(filename);
        int linenum = message->GetLineNumber(context).FromJust();
        fprintf(stderr, "%s:%i: %s\n", file_string, linenum, exception_string);
        v8::String::Utf8Value sourceline(
            isolate, message->GetSourceLine(context).ToLocalChecked());
        const char* sourceline_string = ToCString(sourceline);
        fprintf(stderr, "%s\n", sourceline_string);
        int start = message->GetStartColumn(context).FromJust();
        for (int i = 0; i < start ; i++) {
            fprintf(stderr, " ");
        }
        int end = message->GetEndColumn(context).FromJust();
        for (int i = start; i < end; i++) {
            fprintf(stderr, "^");
        }
        fprintf(stderr, "\n");
        v8::Local<v8::Value> stack_trace_string;
        if (try_catch->StackTrace(context).ToLocal(&stack_trace_string) &&
            stack_trace_string->IsString() &&
            v8::Local<v8::String>::Cast(stack_trace_string)->Length() > 0) {
            v8::String::Utf8Value stack_trace(isolate, stack_trace_string);
            const char* stack_trace_string = ToCString(stack_trace);
            fprintf(stderr, "%s\n", stack_trace_string);
        }
    }
    
}

void Version(v8::FunctionCallbackInfo<v8::Value>& args) {
    args.GetReturnValue().Set(
        v8::String::NewFromUtf8(args.GetIsolate(), v8::V8::GetVersion(),
        v8::NewStringType::kNormal).ToLocalChecked());
}
    
} // namespace ll
