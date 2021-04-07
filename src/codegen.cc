#include "codegen.hh"
#include "debug.hh"
#include "interpreter.hh"
#include "module.hh"
#include "int.hh"
#include "null.hh"
#include "bool.hh"
#include "str.hh"
#include "error.hh"
#include <iostream>

// TODO : Check nothrow on new
// TODO : Gencode bool for error

// TODO : Rm debug
#define PUSH_CODE(DATA) module->frame->code.push_back(DATA); \
    cout << "> " << #DATA << endl;

#define ADD_CONST(DATA) module->frame->add_const(DATA); \
    // cout << "* " << #DATA << endl;

using namespace OpCode;
using namespace std;
using namespace ast;

#define DEBUG_FATAL(MOD, LOC, MSG)                                             \
    debug_err((MOD)->filepath + ":" + to_string(LOC) + " : " + (MSG));         \
    return

void gen_module_code(AstModule *ast, ModuleObject *module) {
    ast->gen_code(module);
}

void AstModule::gen_code(ModuleObject *module) {
    content->gen_code(module);

    // Return null at the end
    PUSH_CODE(LoadConst);
    auto off_null = ADD_CONST(null);
    PUSH_CODE(off_null);
    PUSH_CODE(Return);
}

// --- Stmts ---
void Block::gen_code(ModuleObject *module) {
    for (auto stmt : stmts) stmt->gen_code(module);
}

void ExpStmt::gen_code(ModuleObject *module) {
    // Generate expression
    exp->gen_code(module);

    // Remove result
    PUSH_CODE(Pop);
}

// --- Exps ---
void Set::gen_code(ModuleObject *module) {
    exp->gen_code(module);

    PUSH_CODE(StoreVar);

    auto const_id = new (nothrow) Str(id);

    if (!const_id) {
        THROW_MEMORY_ERROR;

        return;
    }

    auto off_id = ADD_CONST(const_id);
    PUSH_CODE(off_id);
}

void Const::gen_code(ModuleObject *module) {
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

    PUSH_CODE(LoadConst);

    auto const_offset = ADD_CONST(const_val);
    PUSH_CODE(const_offset);
}

void BinExp::gen_code(ModuleObject *module) {
    // TODO
}
