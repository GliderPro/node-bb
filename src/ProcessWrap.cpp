#include <locale>
#include <codecvt>
#include <string>

#define BUILDING_NODE_EXTENSION
#include <node.h>
#include "ProcessWrap.h"

v8::Persistent<v8::Function> ProcessWrap::constructor;

ProcessWrap::ProcessWrap() {}

ProcessWrap::~ProcessWrap() {}

void ProcessWrap::Init(v8::Handle<v8::Object> exports)
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();

    // Prepare constructor template
    v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate, New);
    tpl->SetClassName(v8::String::NewFromUtf8(isolate, "ProcessWrap"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Prototypes
    NODE_SET_PROTOTYPE_METHOD(tpl, "Attach", Attach);

    constructor.Reset(isolate, tpl->GetFunction());
    exports->Set(v8::String::NewFromUtf8(isolate, "ProcessWrap"), tpl->GetFunction());
}

void ProcessWrap::New(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::HandleScope scope(isolate);

    if (args.IsConstructCall()) {
        // Invoked as constructor: `new MyObject(...)`
        ProcessWrap* obj = new ProcessWrap();
        obj->Wrap(args.This());
        args.GetReturnValue().Set(args.This());
    }
    else {
        // Invoked as plain function `DriverControlWrap(...)`, turn into construct call.
        v8::Local<v8::Function> cons = v8::Local<v8::Function>::New(isolate, constructor);
        args.GetReturnValue().Set(cons->NewInstance());
    }
}

/// <summary>
/// Attach to existing process
/// </summary>
/// <param name="pid">Process ID</param>
/// <param name="access">Access mask</param>
/// <returns>Status code</returns>
/// BLACKBONE_API NTSTATUS Attach(DWORD pid, DWORD access = DEFAULT_ACCESS_P);
void ProcessWrap::Attach(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::HandleScope scope(isolate);

    if (args.Length() == 0 || args.Length() > 2)
    {
        isolate->ThrowException(v8::Exception::TypeError(
            v8::String::NewFromUtf8(isolate, "Wrong number of arguments")));
        return;
    }

    ProcessWrap* obj = ObjectWrap::Unwrap<ProcessWrap>(args.Holder());
    NTSTATUS status = STATUS_INVALID_PARAMETER;
    if (args.Length() == 1 && args[0]->IsInt32())
    {

        status = obj->_proc.Attach(args[0]->Int32Value());
    }
    else if (args.Length() == 2 && args[0]->IsInt32() && args[1]->IsInt32())
    {
        status = obj->_proc.Attach(args[0]->Int32Value(), args[1]->Int32Value());
    }
    else
    {
        isolate->ThrowException(v8::Exception::TypeError(
            v8::String::NewFromUtf8(isolate, "Wrong type of arguments")));
        return;
    }

    args.GetReturnValue().Set(v8::Number::New(isolate, status));
}