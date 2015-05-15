#define BUILDING_NODE_EXTENSION
#include <node.h>
#pragma comment(lib, "node")

#include "DriverControlWrap.h"
//#include "ProcessWrap.h"

#include <Process/Process.h>


void Initialize(v8::Handle<v8::Object> target) 
{
    // Attach to the current process (node.exe) so we can set debug permissions
    blackbone::Process proc;
    proc.Attach(GetCurrentProcessId());

    //ProcessWrap::Init(target);
    DriverControlWrap::Init(target);
}

NODE_MODULE(blackbone, Initialize)