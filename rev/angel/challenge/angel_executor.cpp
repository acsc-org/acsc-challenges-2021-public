#include <iostream>
#include <cassert>

#include <angelscript.h>
#include <scriptstdstring/scriptstdstring.h>
#include <scriptbuilder/scriptbuilder.h>
#include <scriptarray/scriptarray.h>

#include "angel.hpp"

#include <unistd.h>

class CBytecodeStream : public asIBinaryStream
{
public:
    CBytecodeStream() { f = NULL; }
    ~CBytecodeStream() { if(f) fclose(f); }

    int Open(const char *filename)
    {
        if(f) return -1;
        f = fopen(filename, "rb");
        if(f == NULL) return -1;
        return 0;
    }

    int Write(const void *ptr, asUINT size) { return -1; }
    int Read(void *ptr, asUINT size) {
        if(size == 0 || f == NULL) return 0; 
        fread(ptr, 1, size, f);
        return 0;
    }
    void Restart() {if(f) fseek(f, 0, SEEK_SET);}

protected:
    FILE *f;
};

void DisassembleFunc(asIScriptFunction* func, asIScriptEngine* engine)
{
    asUINT   length;
    asDWORD *byteCode = func->GetByteCode(&length);
    asDWORD *end = byteCode + length;

    int pos = 0;
    while( byteCode < end )
    {
        asEBCInstr op = asEBCInstr(*(asBYTE*)byteCode);
		int size = asBCTypeSize[asBCInfo[op].type];

		char instbytes[0x10];
		instbytes[0] = '\0';

		char tmp[0x40];
		for (int i = 0; i < size; i++) {
			sprintf(tmp, "%02x ", (unsigned char)byteCode[i]);
			strcat(instbytes, tmp);
        }

		printf("%08x : %-16s : %-16s", pos, instbytes, asBCInfo[op].name);

        switch( asBCInfo[op].type )
        {
			case asBCTYPE_NO_ARG:
				printf("NOARG");
				break;

			case asBCTYPE_W_ARG:
			case asBCTYPE_wW_ARG:
			case asBCTYPE_rW_ARG:
				printf("%08hx", asBC_WORDARG0(byteCode));
				break;
			case asBCTYPE_DW_ARG:
				printf("%08x", asBC_DWORDARG(byteCode));
				break;
			case asBCTYPE_QW_ARG:
				printf("%08lx", asBC_QWORDARG(byteCode));
				break;

			case asBCTYPE_wW_rW_ARG:
			case asBCTYPE_rW_rW_ARG:
			case asBCTYPE_wW_W_ARG:
				printf("%08hx %08hx", asBC_WORDARG0(byteCode), asBC_WORDARG1(byteCode));
				break;
			case asBCTYPE_rW_DW_ARG:
			case asBCTYPE_wW_DW_ARG:
			case asBCTYPE_W_DW_ARG:
				printf("%08hx %08x", asBC_WORDARG0(byteCode), asBC_DWORDARG(byteCode));
				break;
			case asBCTYPE_rW_QW_ARG:
			case asBCTYPE_wW_QW_ARG:
				printf("%08hx %08lx", asBC_WORDARG0(byteCode), asBC_QWORDARG(byteCode));
				break;
			case asBCTYPE_QW_DW_ARG:
				printf("%08lx %08x", asBC_QWORDARG(byteCode), asBC_DWORDARG(byteCode));
				break;

			case asBCTYPE_wW_rW_rW_ARG:
				printf("%08hx %08hx %08hx", asBC_WORDARG0(byteCode), asBC_WORDARG1(byteCode), asBC_SWORDARG2(byteCode));
				break;
			case asBCTYPE_wW_rW_DW_ARG:
			case asBCTYPE_rW_W_DW_ARG:
				printf("%08hx %08hx %08x", asBC_WORDARG0(byteCode), asBC_WORDARG1(byteCode), asBC_DWORDARG(byteCode));
				break;

			case asBCTYPE_rW_DW_DW_ARG:
			case asBCTYPE_DW_DW_ARG:
				printf("UNIMPLEMENTED");
				break;

			default:
				assert(false);
        }

		asIScriptFunction *_func;
        switch( op )
        {
			case asBC_CALLSYS:
				_func = engine->GetGlobalFunctionByIndex(asBC_INTARG(byteCode));
                printf(" %08x ", engine->GetGlobalFunctionCount());
				if (_func != NULL) {
                	printf("%s", _func->GetDeclaration());
				}
                break;
            default:
                break;
        }

		printf("\n");

        byteCode += size;
        pos += size;
    }
}

void ShowDeclaredFunctions(asIScriptModule* mod){
    for (int i = 0; i < (int)mod->GetFunctionCount(); i++) {
       printf("%2d: %s\n", i, mod->GetFunctionByIndex(i)->GetDeclaration());
    }
}

void Disassemble(asIScriptModule* mod, asIScriptEngine *engine) {
    while (true) {
        printf("Functions:\n");
        ShowDeclaredFunctions(mod);

        int functionNumber;
        printf("Enter the number of the function you want to disassemble: ");
        int scanResult = scanf("%d", &functionNumber);
        if (scanResult != 1) {
            break;
        }

        if (0 <= functionNumber && functionNumber < (int)mod->GetFunctionCount()) {
            DisassembleFunc(mod->GetFunctionByIndex(functionNumber), engine);
        } else {
            printf("Invalid number\n");
        }
    }
}

void ExecuteMainFunc(asIScriptModule* mod, asIScriptEngine *engine) {
    asIScriptFunction* func = mod->GetFunctionByDecl("void main()");
    asIScriptContext *ctx = engine->CreateContext();

    ctx->Prepare(func);
    ctx->Execute();
    
    ctx->Release();
}

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

int main(int argc, char* argv[]){
    int r = 0;
    
    asIScriptEngine *engine = asCreateScriptEngine();
    if (engine == 0) {
        std::cout << "Failed to create script engine." << std::endl;
        return -1;
    }

    engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);

    RegisterTypesAndFunctions(engine);

    asIScriptModule* mod = engine->GetModule("Angel", asGM_ALWAYS_CREATE);

    CBytecodeStream stream;
    stream.Open("angel.bytecode");

    r = mod->LoadByteCode(&stream);
    if (r < 0) {
        std::cout << "Failed to load bytecode." << std::endl;
        return -1;
    }

    bool is_disas = false;
    int opt;
    while ((opt = getopt(argc, argv, "dh")) != -1) {
        switch (opt) {
            case 'd':
                is_disas = true;
                break;

            case 'h':
            default:
                printf("Usage: %s [-d] [-h]\n\n", argv[0]);
                printf("Optional arguments:\n");
                printf("  -d \t disassemble bytecode\n");
                printf("  -h \t show usage\n");
                return 0;
        }
    }
    if (is_disas) {
        Disassemble(mod, engine);
    } else {
        ExecuteMainFunc(mod, engine);
    }

    if (g_angelParams) {
        g_angelParams->Release();
    }
    engine->ShutDownAndRelease();

    return 0;
}