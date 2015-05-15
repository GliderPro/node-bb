#include <locale>
#include <codecvt>
#include <string>
#include <memory>

#define BUILDING_NODE_EXTENSION
#include <node.h>
#include <node_buffer.h>
#include "DriverControlWrap.h"

v8::Persistent<v8::Function> DriverControlWrap::constructor;

DriverControlWrap::DriverControlWrap() {}

DriverControlWrap::~DriverControlWrap() {}

void DriverControlWrap::Init(v8::Handle<v8::Object> exports) 
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();

    // Prepare constructor template
    v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate, New);
    tpl->SetClassName(v8::String::NewFromUtf8(isolate, "DriverControlWrap"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Prototypes
    NODE_SET_PROTOTYPE_METHOD(tpl, "EnsureLoaded", EnsureLoaded);
    NODE_SET_PROTOTYPE_METHOD(tpl, "Reload", Reload);
    NODE_SET_PROTOTYPE_METHOD(tpl, "Unload", Unload);
    NODE_SET_PROTOTYPE_METHOD(tpl, "DisableDEP", DisableDEP);
    NODE_SET_PROTOTYPE_METHOD(tpl, "ProtectProcess", ProtectProcess);
    NODE_SET_PROTOTYPE_METHOD(tpl, "PromoteHandle", PromoteHandle);
    NODE_SET_PROTOTYPE_METHOD(tpl, "ReadMem", ReadMem);

    constructor.Reset(isolate, tpl->GetFunction());
    exports->Set(v8::String::NewFromUtf8(isolate, "DriverControlWrap"), tpl->GetFunction());
}

void DriverControlWrap::New(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::HandleScope scope(isolate);

    if (args.IsConstructCall()) {
        // Invoked as constructor: `new MyObject(...)`
        DriverControlWrap* obj = new DriverControlWrap();
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
/// Try to load driver if it isn't loaded
/// </summary>
/// <param name="path">Path to the driver file</param>
/// <returns>Status code</returns>
/// BLACKBONE_API NTSTATUS EnsureLoaded(const std::wstring& path = L"");
void DriverControlWrap::EnsureLoaded(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::HandleScope scope(isolate);

    if (args.Length() > 1) 
    {
        isolate->ThrowException(v8::Exception::TypeError(
            v8::String::NewFromUtf8(isolate, "Wrong number of arguments")));
        return;
    }

    DriverControlWrap* obj = ObjectWrap::Unwrap<DriverControlWrap>(args.Holder());
    NTSTATUS status = STATUS_INVALID_PARAMETER;
    if (args.Length() == 1 && args[0]->IsString())
    {
        v8::String::Utf8Value utf8str(args[0]->ToString());
        std::string utf8name = std::string(*utf8str, utf8str.length());
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        std::wstring utf16name = converter.from_bytes(utf8name);
        status = blackbone::Driver().EnsureLoaded(utf16name);
    }
    else
    {
        status = blackbone::Driver().EnsureLoaded();
    }

    args.GetReturnValue().Set(v8::Number::New(isolate, status));
}

/// <summary>
/// Reload driver
/// </summary>
/// <param name="path">Path to the driver file</param>
/// <returns>Status code</returns>
/// BLACKBONE_API NTSTATUS Reload(std::wstring path = L"");
void DriverControlWrap::Reload(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::HandleScope scope(isolate);

    if (args.Length() > 1)
    {
        isolate->ThrowException(v8::Exception::TypeError(
            v8::String::NewFromUtf8(isolate, "Wrong number of arguments")));
        return;
    }

    DriverControlWrap* obj = ObjectWrap::Unwrap<DriverControlWrap>(args.Holder());
    NTSTATUS status = STATUS_INVALID_PARAMETER;
    if (args.Length() == 1 && args[0]->IsString())
    {
        v8::String::Utf8Value utf8str(args[0]->ToString());
        std::string utf8name = std::string(*utf8str, utf8str.length());
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        std::wstring utf16name = converter.from_bytes(utf8name);
        status = blackbone::Driver().Reload(utf16name);
    }
    else
    {
        status = blackbone::Driver().Reload();
    }

    args.GetReturnValue().Set(v8::Number::New(isolate, status));
}

/// <summary>
/// Unload driver
/// </summary>
/// <returns>Status code</returns>
/// BLACKBONE_API NTSTATUS Unload();
void DriverControlWrap::Unload(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::HandleScope scope(isolate);

    DriverControlWrap* obj = ObjectWrap::Unwrap<DriverControlWrap>(args.Holder());
    NTSTATUS status = blackbone::Driver().Unload();

    args.GetReturnValue().Set(v8::Number::New(isolate, status));
}

/// <summary>
/// Disable DEP for process
/// Has no effect on native x64 processes
/// </summary>
/// <param name="pid">Target PID</param>
/// <returns>Status code</returns>
/// BLACKBONE_API NTSTATUS DisableDEP(DWORD pid);
void DriverControlWrap::DisableDEP(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::HandleScope scope(isolate);

    if (args.Length() != 1)
    {
        isolate->ThrowException(v8::Exception::TypeError(
            v8::String::NewFromUtf8(isolate, "Wrong number of arguments")));
        return;
    }

    DriverControlWrap* obj = ObjectWrap::Unwrap<DriverControlWrap>(args.Holder());
    NTSTATUS status = STATUS_INVALID_PARAMETER;
    if (args[0]->IsInt32())
    {
        status = blackbone::Driver().DisableDEP(args[0]->Int32Value());
    }
    else
    {
        isolate->ThrowException(v8::Exception::TypeError(
            v8::String::NewFromUtf8(isolate, "Wrong type of arguments")));
        return;
    }

    args.GetReturnValue().Set(v8::Number::New(isolate, status));
}

/// <summary>
/// Change process protection flag
/// </summary>
/// <param name="pid">Target PID</param>
/// <param name="enable">true to enable protection, false to disable</param>
/// <returns>Status code</returns>
/// BLACKBONE_API NTSTATUS ProtectProcess(DWORD pid, bool enable);
void DriverControlWrap::ProtectProcess(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::HandleScope scope(isolate);

    if (args.Length() != 2)
    {
        isolate->ThrowException(v8::Exception::TypeError(
            v8::String::NewFromUtf8(isolate, "Wrong number of arguments")));
        return;
    }

    DriverControlWrap* obj = ObjectWrap::Unwrap<DriverControlWrap>(args.Holder());
    NTSTATUS status = STATUS_INVALID_PARAMETER;
    if (args[0]->IsInt32() && args[1]->IsBoolean())
    {
        status = blackbone::Driver().ProtectProcess(args[0]->Int32Value(),args[1]->BooleanValue());
    }
    else
    {
        isolate->ThrowException(v8::Exception::TypeError(
            v8::String::NewFromUtf8(isolate, "Wrong type of arguments")));
        return;
    }

    args.GetReturnValue().Set(v8::Number::New(isolate, status));
}

/// <summary>
/// Change handle access rights
/// </summary>
/// <param name="pid">Target PID.</param>
/// <param name="handle">Handle</param>
/// <param name="access">New access</param>
/// <returns>Status code</returns>
/// BLACKBONE_API NTSTATUS PromoteHandle(DWORD pid, HANDLE handle, DWORD access);
void DriverControlWrap::PromoteHandle(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::HandleScope scope(isolate);

    if (args.Length() != 3)
    {
        isolate->ThrowException(v8::Exception::TypeError(
            v8::String::NewFromUtf8(isolate, "Wrong number of arguments")));
        return;
    }

    DriverControlWrap* obj = ObjectWrap::Unwrap<DriverControlWrap>(args.Holder());
    NTSTATUS status = STATUS_INVALID_PARAMETER;
    if (args[0]->IsInt32() && args[1]->IsNumber() && args[2]->IsInt32())
    {
        status = blackbone::Driver().PromoteHandle(args[0]->Int32Value(), 
            reinterpret_cast<HANDLE>(args[1]->IntegerValue()), args[2]->Int32Value());
    }
    else
    {
        isolate->ThrowException(v8::Exception::TypeError(
            v8::String::NewFromUtf8(isolate, "Wrong type of arguments")));
        return;
    }

    args.GetReturnValue().Set(v8::Number::New(isolate, status));
}

/// <summary>
/// Allocate virtual memory
/// </summary>
/// <param name="pid">Tarhet PID</param>
/// <param name="base">Desired base. If 0 address is chosed by the system</param>
/// <param name="size">Region size</param>
/// <param name="type">Allocation type - MEM_RESERVE/MEM_COMMIT</param>
/// <param name="protection">Memory protection</param>
/// <returns>Status code</returns>
/// BLACKBONE_API NTSTATUS AllocateMem(DWORD pid, ptr_t& base, ptr_t& size, DWORD type, DWORD protection, bool physical = false);

/// <summary>
/// Free virtual memory
/// </summary>
/// <param name="pid">Tarhet PID</param>
/// <param name="base">Desired base. If 0 address is chosed by the system</param>
/// <param name="size">Region size</param>
/// <param name="type">Free type - MEM_RELEASE/MEM_DECOMMIT</param>
/// <returns>Status code</returns>
/// BLACKBONE_API NTSTATUS FreeMem(DWORD pid, ptr_t base, ptr_t size, DWORD type);

/// <summary>
/// Read process memory
/// </summary>
/// <param name="pid">Target PID</param>
/// <param name="base">Target base</param>
/// <param name="size">Data size</param>
/// <param name="buffer">Buffer address</param>
/// <returns>Status code</returns>
/// BLACKBONE_API NTSTATUS ReadMem(DWORD pid, ptr_t base, ptr_t size, PVOID buffer);
void DriverControlWrap::ReadMem(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::HandleScope scope(isolate);

    if (args.Length() != 3)
    {
        isolate->ThrowException(v8::Exception::TypeError(
            v8::String::NewFromUtf8(isolate, "Wrong number of arguments")));
        return;
    }

    DriverControlWrap* obj = ObjectWrap::Unwrap<DriverControlWrap>(args.Holder());
    NTSTATUS status = STATUS_INVALID_PARAMETER;
    if (args[0]->IsInt32() && args[1]->IsNumber() && args[2]->IsObject())
    {
        DWORD pid = args[0]->Int32Value();
        blackbone::ptr_t addr = static_cast<blackbone::ptr_t>(args[1]->IntegerValue());
        v8::Local<v8::Object> buffer = args[2]->ToObject();
        blackbone::ptr_t size = node::Buffer::Length(buffer);
        char *ptr = node::Buffer::Data(buffer);
        status = blackbone::Driver().ReadMem(pid, addr, size, ptr);
        if (NT_SUCCESS(status))
        {
            OutputDebugStringW(L"foo");
        }
    }
    else
    {
        isolate->ThrowException(v8::Exception::TypeError(
            v8::String::NewFromUtf8(isolate, "Wrong type of arguments")));
        return;
    }

    args.GetReturnValue().Set(v8::Number::New(isolate, status));
}

/// <summary>
/// Write process memory
/// </summary>
/// <param name="pid">Target PID</param>
/// <param name="base">Target base</param>
/// <param name="size">Data size</param>
/// <param name="buffer">Buffer address</param>
/// <returns>Status code</returns>
/// BLACKBONE_API NTSTATUS WriteMem(DWORD pid, ptr_t base, ptr_t size, PVOID buffer);

/// <summary>
/// Change memory protection
/// </summary>
/// <param name="pid">Target PID.</param>
/// <param name="base">Regiod base address</param>
/// <param name="size">Region size</param>
/// <param name="protection">New protection</param>
/// <returns>Status code</returns>
/// BLACKBONE_API NTSTATUS ProtectMem(DWORD pid, ptr_t base, ptr_t size, DWORD protection);

/// <summary>
/// Maps target process memory into current process
/// </summary>
/// <param name="pid">Target PID</param>
/// <param name="pipeName">Pipe name to use for hook data transfer</param>
/// <param name="mapSections">The map sections.</param>
/// <param name="result">Results</param>
/// <returns>Status code </returns>
/// BLACKBONE_API NTSTATUS MapMemory(DWORD pid, const std::wstring& pipeName, bool mapSections, MapMemoryResult& result);

/// <summary>
/// Maps single memory region into current process
/// </summary>
/// <param name="pid">Target PID</param>
/// <param name="base">Region base address</param>
/// <param name="size">Region size</param>
/// <param name="result">Mapped region info</param>
/// <returns>Status code</returns>
/// BLACKBONE_API NTSTATUS MapMemoryRegion(DWORD pid, ptr_t base, uint32_t size, MapMemoryRegionResult& result);

/// <summary>
/// Unmap memory of the target process from current
/// </summary>
/// <param name="pid">Target PID</param>
/// <returns>Status code</returns>
/// BLACKBONE_API NTSTATUS UnmapMemory(DWORD pid);

/// <summary>
/// Unmap single memory region
/// If unmapped region size is smaller than the size specified during map, function will return info about
/// 2 regions that emerged after unmap
/// </summary>
/// <param name="pid">Target PID</param>
/// <param name="base">Region base</param>
/// <param name="size">Region size</param>
/// <param name="result">Unampped region info</param>
/// <returns>Status code</returns>
/// BLACKBONE_API NTSTATUS UnmapMemoryRegion(DWORD pid, ptr_t base, uint32_t size);
