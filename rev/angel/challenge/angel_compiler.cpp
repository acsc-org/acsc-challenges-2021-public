#include <iostream>
#include <cassert>

#include <angelscript.h>
#include <scriptstdstring/scriptstdstring.h>
#include <scriptbuilder/scriptbuilder.h>

#include "angel.hpp"

class CBytecodeStream : public asIBinaryStream
{
public:
    CBytecodeStream() { f = NULL; }
    ~CBytecodeStream() { if(f) fclose(f); }

    int Open(const char *filename)
    {
        if(f) return -1;
        f = fopen(filename, "wb");
        if(f == NULL) return -1;
        return 0;
    }

    int Write(const void *ptr, asUINT size) 
    {
        if(size == 0 || f == NULL) return 0; 
        fwrite(ptr, 1, size, f);
        return 0;
    }
    int Read(void *, asUINT) { return -1; }

protected:
    FILE *f;
};

void MessageCallback(const asSMessageInfo *msg, void *param)
{
    const char *type = "ERR ";
    if (msg->type == asMSGTYPE_WARNING) {
        type = "WARN";
    } else if (msg->type == asMSGTYPE_INFORMATION) {
        type = "INFO";
    }

    printf("%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
}

int main(){
    int r = 0;

    asIScriptEngine *engine = asCreateScriptEngine();
    if( engine == 0 ) {
        std::cout << "Failed to create script engine." << std::endl;
        return -1;
    }

    engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);

    RegisterTypesAndFunctions(engine);

    CScriptBuilder builder;

    builder.StartNewModule(engine, "Angel");
    if( builder.AddSectionFromFile("angel.as") < 0) {
        std::cout << "Failed to load script file." << std::endl;
        return -1;
    }

    builder.BuildModule();

    asIScriptModule* mod = engine->GetModule("Angel");
    asIScriptContext *ctx = engine->CreateContext();

    CBytecodeStream stream;
    stream.Open("angel.bytecode");
    mod->SaveByteCode(&stream, true);

    ctx->Release();
    engine->ShutDownAndRelease();

    return 0;
}