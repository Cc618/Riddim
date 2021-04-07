#include "codegen.hh"
#include "debug.hh"
#include "interpreter.hh"
#include "module.hh"
#include "int.hh"
#include "str.hh"
#include "error.hh"
#include <iostream>

using namespace OpCode;
using namespace std;
using namespace ast;

#define DEBUG_FATAL(MOD, LOC, MSG)                                             \
    debug_err((MOD)->filepath + ":" + to_string(LOC) + " : " + (MSG));         \
    return

void gen_module_code(AstModule *ast, ModuleObject *module) {
    ast->content->gen_code(module);
}

void AstModule::gen_code(ModuleObject *module) {
    code_t &code = module->frame->code;

    // TODO
    // code.push_back();
}

// --- Stmts ---
void Block::gen_code(ModuleObject *module) {
    code_t &code = module->frame->code;

    // TODO
    // code.push_back();
}

void ExpStmt::gen_code(ModuleObject *module) {
    code_t &code = module->frame->code;

    // TODO
    // code.push_back();
}

// --- Exps ---
void Set::gen_code(ModuleObject *module) {
    code_t &code = module->frame->code;

    // TODO
    // code.push_back();
}

void Const::gen_code(ModuleObject *module) {
    code_t &code = module->frame->code;
    Object *const_val;

    switch (type) {
    case Const::Type::Int:
        const_val = new ::Int(get<int_t>(val));
        break;

    case Const::Type::Str:
        const_val = new ::Str(get<str_t>(val));
        break;

    default:
        DEBUG_FATAL(module, fileline, "Const::gen_code : Unknown type");
    }

    if (!const_val) {
        THROW_MEMORY_ERROR;

        return;
    }

    // Add this value
    auto const_offset = module->frame->add_const(const_val);

    // Add opcode
    code.push_back(LoadConst);
    code.push_back(const_offset);
}

void BinExp::gen_code(ModuleObject *module) {
    code_t &code = module->frame->code;

    // TODO
    // code.push_back(BinAdd);
}
