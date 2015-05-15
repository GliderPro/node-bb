#ifndef PROCESSWRAP_H
#define PROCESSWRAP_H

#define BUILDING_NODE_EXTENSION
#include <node.h>
#include <node_object_wrap.h>
#include <Process/Process.h>

class ProcessWrap : public node::ObjectWrap {
public:
    static void Init(v8::Handle<v8::Object> exports);

private:
    explicit ProcessWrap();
    ~ProcessWrap();
    static v8::Persistent<v8::Function> constructor;
    static void New(const v8::FunctionCallbackInfo<v8::Value>& args);

    // Wrapped methods
    static void Attach(const v8::FunctionCallbackInfo<v8::Value>& args);

    // Wrapped object
    blackbone::Process _proc;
};

#endif