#ifndef DRIVERCONTROLWRAP_H
#define DRIVERCONTROLWRAP_H

#define BUILDING_NODE_EXTENSION
#include <node.h>
#include <node_object_wrap.h>
#include <DriverControl/DriverControl.h>

class DriverControlWrap : public node::ObjectWrap {
public:
    static void Init(v8::Handle<v8::Object> exports);

private:
    explicit DriverControlWrap();
    ~DriverControlWrap();
    static v8::Persistent<v8::Function> constructor;
    static void New(const v8::FunctionCallbackInfo<v8::Value>& args);

    // Wrapped methods
    static void EnsureLoaded(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Reload(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Unload(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void DisableDEP(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void ProtectProcess(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void PromoteHandle(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void ReadMem(const v8::FunctionCallbackInfo<v8::Value>& args);

    // Wrapped object
    // DriverControl is entirely static so we don't need to wrap it
};

#endif