#include <iostream>
#include <iomanip>
#include <fstream>
#include <cassert>
#include <random>

#include <scriptarray/scriptarray.h>
#include <scriptfile/scriptfile.h>

#define KEY_SIZE 16
#define MSG_SIZE 48
// #define PARAM_BUF_SIZE (KEY_SIZE + MSG_SIZE)
#define PARAM_BUF_SIZE MSG_SIZE

CScriptArray *g_angelParams = NULL;

void Print(std::string &str)
{
    std::cout << str;
}

void PrintUint8AsHex(uint val, bool isNewLine=false)
{
    if (isNewLine) {
        std::cout << std::setfill('0') << std::setw(2) << std::hex << val << std::endl;
    } else {
        std::cout << std::setfill('0') << std::setw(2) << std::hex << val;
    }
}

CScriptArray *GetAngelParams()
{
    if (g_angelParams) {
        g_angelParams->AddRef();
        return g_angelParams;
    }

    asIScriptContext *ctx = asGetActiveContext();
    asIScriptEngine *engine = ctx->GetEngine();

    asITypeInfo *arrayType = engine->GetTypeInfoById(engine->GetTypeIdByDecl("array<uint8>"));
    g_angelParams = CScriptArray::Create(arrayType, (asUINT)PARAM_BUF_SIZE);

    // std::random_device rng;
    // for(int i = 0; i < KEY_SIZE; i++){
    //     // Add the arg to the array
    //     // g_commandLineArgs->Resize(g_commandLineArgs->GetSize()+1);
    //     // ((string*)g_commandLineArgs->At(n))->assign(g_argv[n]);
    //     *(uint8_t *)(g_angelParams->At(i)) = (uint8_t)rng();
    // }
    std::ifstream flagFile("flag.txt", std::ios::binary | std::ios::in);
    if (!flagFile) {
        exit(1);
    }
    char msg[MSG_SIZE];
    flagFile.read(msg, MSG_SIZE);
    // for(int i = 0; i < MSG_SIZE; i++){
    //     *(uint8_t *)(g_angelParams->At(KEY_SIZE + i)) = (uint8_t)msg[i];
    // }
    for(int i = 0; i < MSG_SIZE; i++){
        *(uint8_t *)(g_angelParams->At(i)) = (uint8_t)msg[i];
    }

    // Return the array by handle
    g_angelParams->AddRef();
    return g_angelParams;
}

void RegisterTypesAndFunctions(asIScriptEngine *engine)
{
    RegisterStdString(engine);
    RegisterScriptArray(engine, false);
    RegisterScriptFile(engine);
    
    int r = 0;
    r = engine->RegisterGlobalFunction("void Print(const string &in)", asFUNCTION(Print), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void Print(uint8 val, bool isNewLine=false)", asFUNCTION(PrintUint8AsHex), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("array<uint8> @GetAngelParams()", asFUNCTION(GetAngelParams), asCALL_CDECL); assert(r >= 0);
}
