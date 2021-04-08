#include "codegen.hh"
#include "bool.hh"
#include "debug.hh"
#include "error.hh"
#include "int.hh"
#include "interpreter.hh"
#include "module.hh"
#include "null.hh"
#include "str.hh"
#include <iostream>

// TODO : Check nothrow on new
// TODO : Gencode bool for error

// TODO : Rm debug
#define PUSH_CODE(DATA)                                                        \
    module->frame->code.push_back(DATA);                                       \
    // cout << "> " << #DATA << endl;

#define ADD_CONST(DATA)                                                        \
    module->frame->add_const(DATA);                                            \
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
    for (auto stmt : stmts)
        stmt->gen_code(module);
}

void Stmt::gen_code(ModuleObject *module) {
    module->frame->mark_line(fileline);
}

void IfStmt::gen_code(ModuleObject *module) {
    Stmt::gen_code(module);

    auto &code = module->frame->code;

    // The pseudo code of the generation
    // condition is false ? goto else
    // if body
    // goto finally
    // else:
    // else body
    // finally:

    // Generate condition
    condition->gen_code(module);

    // If false, goto elseaddr
    PUSH_CODE(JmpFalse);

    // Save offset to change the nop (placeholder) to the address
    // of the else section
    auto elseaddr_offset = code.size();
    PUSH_CODE(Nop);

    // If true, go here
    ifbody->gen_code(module);

    // Jump after the else (finally section)
    PUSH_CODE(Jmp);
    auto ifaddr_offset = code.size();
    PUSH_CODE(Nop);

    // If false, go here (else)
    code[elseaddr_offset] = code.size();

    if (elsebody) {
        elsebody->gen_code(module);
    }

    // Jump there at the end (finally)
    code[ifaddr_offset] = code.size();
}

void ExpStmt::gen_code(ModuleObject *module) {
    Stmt::gen_code(module);

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

void Attr::gen_code(ModuleObject *module) {
    exp->gen_code(module);

    PUSH_CODE(LoadAttr);

    auto const_attr = new (nothrow) Str(attr);

    if (!const_attr) {
        THROW_MEMORY_ERROR;

        return;
    }

    auto off_attr = ADD_CONST(const_attr);
    PUSH_CODE(off_attr);
}

void Id::gen_code(ModuleObject *module) {
    Object *name = new Str(id);

    if (!name) {
        THROW_MEMORY_ERROR;

        return;
    }

    PUSH_CODE(LoadVar);

    auto name_offset = ADD_CONST(name);
    PUSH_CODE(name_offset);
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

    case Const::Type::True:
        const_val = istrue;
        break;

    case Const::Type::False:
        const_val = isfalse;
        break;

    case Const::Type::Null:
        const_val = null;
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
    left->gen_code(module);
    right->gen_code(module);

    switch (op) {
    case BinExp::Or:
        PUSH_CODE(BinBool);
        PUSH_CODE((opcode_t)BoolBinOp::Or);
        break;

    case BinExp::And:
        PUSH_CODE(BinBool);
        PUSH_CODE((opcode_t)BoolBinOp::And);
        break;

    case BinExp::Equal:
        PUSH_CODE(BinCmp);
        PUSH_CODE((opcode_t)CmpOp::Equal);
        break;

    case BinExp::Lesser:
        PUSH_CODE(BinCmp);
        PUSH_CODE((opcode_t)CmpOp::Lesser);
        break;

    case BinExp::Greater:
        PUSH_CODE(BinCmp);
        PUSH_CODE((opcode_t)CmpOp::Greater);
        break;

    case BinExp::LesserEqual:
        PUSH_CODE(BinCmp);
        PUSH_CODE((opcode_t)CmpOp::LesserEqual);
        break;

    case BinExp::GreaterEqual:
        PUSH_CODE(BinCmp);
        PUSH_CODE((opcode_t)CmpOp::GreaterEqual);
        break;

    case BinExp::Add:
        PUSH_CODE(BinAdd);
        break;

    case BinExp::Mul:
        PUSH_CODE(BinMul);
        break;

    default:
        DEBUG_FATAL(module, fileline, "BinExp::gen_code : Unknown op");
    }
}

void UnaExp::gen_code(ModuleObject *module) {
    exp->gen_code(module);

    switch (op) {
    case UnaExp::Not:
        PUSH_CODE(UnaNot);
        break;

    default:
        DEBUG_FATAL(module, fileline, "UnaExp::gen_code : Unknown op");
    }
}
