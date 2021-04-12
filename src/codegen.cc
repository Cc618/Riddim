#include "codegen.hh"
#include "bool.hh"
#include "debug.hh"
#include "error.hh"
#include "code.hh"
#include "int.hh"
#include "interpreter.hh"
#include "module.hh"
#include "function.hh"
#include "null.hh"
#include "str.hh"
#include "code.hh"
#include <iostream>

// TODO : Rm debug
#define PUSH_CODE(DATA)                                                        \
    _code->code.push_back(DATA);                                               \
    // cout << "> " << #DATA << " (" << (DATA) << ")" << endl;

#define ADD_CONST(DATA)                                                        \
    _code->add_const(DATA);                                                    \
    // cout << "* " << #DATA << endl;

using namespace OpCode;
using namespace std;
using namespace ast;

#define DEBUG_FATAL(MOD, LOC, MSG)                                             \
    debug_err((MOD)->filepath + ":" + to_string(LOC) + " : " + (MSG));         \
    return

// Finalize the generation of a function's code body
//
static void finalize_function_code(Module *module, Code *_code) {
    // Add last return statement (with null)
    PUSH_CODE(Return);
    auto off_null = ADD_CONST(null);
    PUSH_CODE(off_null);
}

bool gen_module_code(AstModule *ast, ModuleObject *module) {
    try {
        ast->gen_code(module, module->code);

        if (on_error())
            return false;

        return true;
    } catch (...) {
        return false;
    }
}

void AstModule::gen_code(Module *module, Code *_code) {
    content->gen_code(module, _code);

    // Return null at the end
    PUSH_CODE(LoadConst);
    auto off_null = ADD_CONST(null);
    PUSH_CODE(off_null);
    PUSH_CODE(Return);
}

// --- Decls ---
void Block::gen_code(Module *module, Code *_code) {
    for (auto stmt : stmts)
        stmt->gen_code(module, _code);
}

void FnDecl::gen_code(Module *module, Code *_code) {
    // TODO A : Update, create symbol in code + not an expression
    auto fncode = Code::New();

    // TODO : Throw
    if (!fncode) return;

    // Generate body within the function's code
    body->gen_code(module, fncode);

    finalize_function_code(module, fncode);

    auto fn = CodeFunction::New(fncode, name);

    auto off_fn = ADD_CONST(fn);

    // Load it
    PUSH_CODE(LoadConst);
    PUSH_CODE(off_fn);

    // Store it
    PUSH_CODE(StoreVar);

    auto const_name = new (nothrow) Str(name);

    if (!const_name) {
        THROW_MEMORY_ERROR;

        return;
    }

    auto off_name = ADD_CONST(const_name);
    PUSH_CODE(off_name);
}

// --- Stmts ---
void Stmt::gen_code(Module *module, Code *_code) {
    _code->mark_line(fileline);
}

void IfStmt::gen_code(Module *module, Code *_code) {
    Stmt::gen_code(module, _code);

    auto &code = _code->code;

    // The pseudo code of the generation
    // condition is false ? goto else
    // if body
    // goto finally
    // else:
    // else body
    // finally:

    // Generate condition
    condition->gen_code(module, _code);

    // If false, goto elseaddr
    PUSH_CODE(JmpFalse);

    // Save offset to change the nop (placeholder) to the address
    // of the else section
    auto elseaddr_offset = code.size();
    PUSH_CODE(Nop);

    // If true, go here
    ifbody->gen_code(module, _code);

    // Jump after the else (finally section)
    PUSH_CODE(Jmp);
    auto ifaddr_offset = code.size();
    PUSH_CODE(Nop);

    // If false, go here (else)
    code[elseaddr_offset] = code.size();

    if (elsebody) {
        elsebody->gen_code(module, _code);
    }

    // Jump there at the end (finally)
    code[ifaddr_offset] = code.size();
}

void WhileStmt::gen_code(Module *module, Code *_code) {
    Stmt::gen_code(module, _code);

    auto &code = _code->code;

    // The pseudo code of the generation
    // start:
    // condition is false ? goto finally
    // body
    // goto start
    // finally:

    // Generate condition
    auto start_offset = code.size();
    condition->gen_code(module, _code);

    // If false, goto finally
    PUSH_CODE(JmpFalse);
    auto finally_offset = code.size();
    PUSH_CODE(Nop);

    // If true, execute body
    body->gen_code(module, _code);

    // Jump at the start section (loop again)
    PUSH_CODE(Jmp);
    PUSH_CODE(start_offset);

    // Finally section
    code[finally_offset] = code.size();
}

void CallExp::gen_code(Module *module, Code *_code) {
    // Push exp
    exp->gen_code(module, _code);

    // No args
    if (args.empty() && kwargs.empty()) {
        // Tiny optimization, call CallProc to avoid creating empty list
        PUSH_CODE(CallProc);
    } else if (kwargs.empty()) {
        // Only positional args
        // Generate expressions
        for (auto arg : args)
            arg->gen_code(module, _code);

        // Make args
        PUSH_CODE(Pack);
        PUSH_CODE(args.size());

        // Call
        PUSH_CODE(Call);
    } else {
        // Keyword and possibly positional args
        // Generate args
        for (auto arg : args)
            arg->gen_code(module, _code);

        // Make args
        PUSH_CODE(Pack);
        PUSH_CODE(args.size());

        // Generate kwargs
        for (const auto &[id, val] : kwargs) {
            auto const_id = new (nothrow) Str(id);

            // TODO : Throw
            if (!const_id) {
                THROW_MEMORY_ERROR;

                return;
            }

            // Load id
            PUSH_CODE(LoadConst);
            auto off_id = ADD_CONST(const_id);
            PUSH_CODE(off_id);

            val->gen_code(module, _code);
        }

        // Make kwargs
        PUSH_CODE(PackMap);
        PUSH_CODE(kwargs.size());

        // Call
        PUSH_CODE(CallKw);
    }
}

void ExpStmt::gen_code(Module *module, Code *_code) {
    Stmt::gen_code(module, _code);

    // Generate expression
    exp->gen_code(module, _code);

    // Remove result
    PUSH_CODE(Pop);
}

// --- Exps ---
void Set::gen_code(Module *module, Code *_code) {
    exp->gen_code(module, _code);
    target->gen_code(module, _code);
}

void VecLiteral::gen_code(Module *module, Code *_code) {
    for (auto exp : exps)
        exp->gen_code(module, _code);

    PUSH_CODE(Pack);
    PUSH_CODE(exps.size());
}

void MapLiteral::gen_code(Module *module, Code *_code) {
    for (const auto &[k, v] : kv) {
        k->gen_code(module, _code);
        v->gen_code(module, _code);
    }

    PUSH_CODE(PackMap);
    PUSH_CODE(kv.size());
}

void Attr::gen_code(Module *module, Code *_code) {
    exp->gen_code(module, _code);

    PUSH_CODE(LoadAttr);

    auto const_attr = new (nothrow) Str(attr);

    if (!const_attr) {
        THROW_MEMORY_ERROR;

        return;
    }

    auto off_attr = ADD_CONST(const_attr);
    PUSH_CODE(off_attr);
}

void Indexing::gen_code(Module *module, Code *_code) {
    container->gen_code(module, _code);
    index->gen_code(module, _code);
    PUSH_CODE(LoadIndex);
}

void Id::gen_code(Module *module, Code *_code) {
    Object *name = new (nothrow) Str(id);

    // TODO : Throw
    if (!name) {
        THROW_MEMORY_ERROR;

        return;
    }

    PUSH_CODE(LoadVar);

    auto name_offset = ADD_CONST(name);
    PUSH_CODE(name_offset);
}

void Const::gen_code(Module *module, Code *_code) {
    Object *const_val;

    switch (type) {
    case Const::Type::Int:
        const_val = new (nothrow)::Int(get<int_t>(val));
        break;

    case Const::Type::Str:
        const_val = new (nothrow)::Str(get<str_t>(val));
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

void BinExp::gen_code(Module *module, Code *_code) {
    left->gen_code(module, _code);
    right->gen_code(module, _code);

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

    case BinExp::Is:
        PUSH_CODE(BinIs);
        break;

    case BinExp::IsNot:
        PUSH_CODE(BinIs);
        PUSH_CODE(UnaNot);
        break;

    default:
        DEBUG_FATAL(module, fileline, "BinExp::gen_code : Unknown op");
    }
}

void UnaExp::gen_code(Module *module, Code *_code) {
    exp->gen_code(module, _code);

    switch (op) {
    case UnaExp::Not:
        PUSH_CODE(UnaNot);
        break;

    default:
        DEBUG_FATAL(module, fileline, "UnaExp::gen_code : Unknown op");
    }
}

// --- Targets ---
void IdTarget::gen_code(Module *module, Code *_code) {
    PUSH_CODE(StoreVar);

    auto const_id = new (nothrow) Str(id);

    if (!const_id) {
        THROW_MEMORY_ERROR;

        return;
    }

    auto off_id = ADD_CONST(const_id);
    PUSH_CODE(off_id);
}

void IndexingTarget::gen_code(Module *module, Code *_code) {
    // Note that indexing->gen_code is not used since it is useful
    // to load not to store the value
    indexing->container->gen_code(module, _code);
    indexing->index->gen_code(module, _code);
    PUSH_CODE(StoreIndex);
}

void AttrTarget::gen_code(Module *module, Code *_code) {
    // Note that attr->gen_code is not used since it is useful
    // to load not to store the value
    // Load object
    attr->exp->gen_code(module, _code);

    // Add attribute name constant
    auto const_attr = new (nothrow) Str(attr->attr);

    if (!const_attr) {
        THROW_MEMORY_ERROR;

        return;
    }

    PUSH_CODE(StoreAttr);
    auto off_attr = ADD_CONST(const_attr);
    PUSH_CODE(off_attr);
}
