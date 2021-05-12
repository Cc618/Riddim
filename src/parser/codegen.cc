#include "codegen.hh"
#include "bool.hh"
#include "code.hh"
#include "debug.hh"
#include "error.hh"
#include "function.hh"
#include "int.hh"
#include "interpreter.hh"
#include "module.hh"
#include "null.hh"
#include "program.hh"
#include "str.hh"
#include <iostream>
#include <unordered_set>

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
    PUSH_CODE(LoadConst);
    auto off_null = ADD_CONST(null);
    PUSH_CODE(off_null);
    // Add last return statement (with null)
    PUSH_CODE(Return);
}

bool gen_module_code(AstModule *ast, ModuleObject *module) {
    try {
        ast->gen_code(module, module->code);

        if (on_error())
            return false;

        return true;
    } catch (const CodeGenException &e) {
        Program::instance->errout << "Code generation error : " << e.what()
                                  << endl;

        return false;
    } catch (...) {
        Program::instance->errout
            << "Fatal internal error during code generation" << endl;

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
    auto fncode = Code::New(module->filepath);

    if (!fncode) {
        throw CodeGenException("Can't allocate memory", module->filepath,
                               fileline);
    }

    // Generate body within the function's code
    body->gen_code(module, fncode);

    finalize_function_code(module, fncode);

    auto target_str = target ? target->to_str() : "";
    auto name = target_str.empty() ? "<anonymous>" : move(target_str);

    auto fn = Function::New(fncode, name, nullptr, doc);
    fn->n_required_args = args->n_required;

    // To disallow functions with same arguments (fn f(a, a) ...)
    unordered_set<str_t> ids;

    // Set up positional args
    for (const auto &[argid, argdefault] : args->args) {
        if (ids.find(argid) != ids.end()) {
            throw CodeGenException(
                "Function " + name + "Multiple arguments with the same name (" +
                    argid + ") disallowed",
                module->filepath, fileline);
        }

        ids.insert(argid);

        // No default
        if (!argdefault) {
            fn->args.push_back({argid, nullptr});
        } else {
            // Generate code to push the arg on the TOS
            Code *default_code = Code::New(module->filepath);
            if (!default_code) {
                return;
            }

            default_code->start_lineno = fileline;

            argdefault->gen_code(module, default_code);
            default_code->code.push_back(Return);

            fn->args.push_back({argid, default_code});
        }
    }

    auto off_fn = ADD_CONST(fn);

    // Load it
    PUSH_CODE(LoadConst);
    PUSH_CODE(off_fn);

    // Bind it
    if (islambda)
        PUSH_CODE(BindLambda);

    // Store it
    if (target)
        target->gen_code(module, _code);
}

// --- Stmts ---
void Stmt::gen_code(Module *module, Code *_code) { _code->mark_line(fileline); }

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

void TryStmt::gen_code(Module *module, Code *_code) {
    Stmt::gen_code(module, _code);

    auto &code = _code->code;

    // Generation :
    // 1. Try
    // PushTryBlock
    // trybody content
    // PopTryBlock
    // Jmp End
    // 2. Catch (repeated for all catch clauses)
    // Load target type
    // Try to catch if matches, otherwise, go to next clause or end
    // catchbody
    // Jmp End
    // 3. Uncaught (if no catch matches)
    // Rethrow
    // 4. End

    // Each offset where the opcode needs to be the end offset
    vector<size_t> set_end_offsets;

    // 1. try
    // Set up block
    PUSH_CODE(PushTryBlock);
    auto pushtry_offset = code.size();
    PUSH_CODE(Nop);

    trybody->gen_code(module, _code);

    PUSH_CODE(PopTryBlock);

    // Jump to end
    PUSH_CODE(Jmp);
    set_end_offsets.push_back(code.size());
    PUSH_CODE(Nop);

    // 2. Catch
    // Set first catch offset
    code[pushtry_offset] = code.size();

    size_t last_catch_jmp_offset = 0;
    for (auto &catchbody : catchbodies) {
        if (last_catch_jmp_offset) {
            code[last_catch_jmp_offset] = code.size();
        }

        // Load the target error type (or null if none)
        if (catchbody.type) {
            _code->mark_line(catchbody.type->fileline);
            catchbody.type->gen_code(module, _code);
        } else {
            PUSH_CODE(LoadConst);
            auto null_offset = ADD_CONST(null);
            PUSH_CODE(null_offset);
        }

        // Test if the error matches the TOS
        PUSH_CODE(CatchError);

        // Gen id
        Object *id =
            catchbody.id.empty()
                ? static_cast<Object *>(null)
                : static_cast<Object *>(Str::New(catchbody.id));

        if (!id) {
            throw CodeGenException("Can't allocate memory", _code->filename,
                                   fileline);
        }

        auto id_offset = ADD_CONST(id);

        PUSH_CODE(id_offset);

        // Next catch location
        last_catch_jmp_offset = code.size();
        PUSH_CODE(Nop);

        catchbody.body->gen_code(module, _code);

        // Jump to end
        PUSH_CODE(CatchTerminate);
        set_end_offsets.push_back(code.size());
        PUSH_CODE(Nop);
    }

    // 3. Uncaught
    if (last_catch_jmp_offset) {
        code[last_catch_jmp_offset] = code.size();
    }

    PUSH_CODE(Rethrow);

    // 4. End
    for (auto offset : set_end_offsets)
        code[offset] = code.size();
}

void ReturnStmt::gen_code(Module *module, Code *_code) {
    Stmt::gen_code(module, _code);

    auto &code = _code->code;

    // Return null
    if (!exp) {
        PUSH_CODE(LoadConst);
        auto off_null = ADD_CONST(null);
        PUSH_CODE(off_null);
    } else
        exp->gen_code(module, _code);

    PUSH_CODE(Return);
}

void LoopControlStmt::gen_code(Module *module, Code *_code) {
    Stmt::gen_code(module, _code);

    auto &code = _code->code;

    if (_code->loop_stack.empty()) {
        throw CodeGenException("Break or continue outside of loop",
                               _code->filename, fileline);
    }

    // Register this control
    _code->loop_stack.back()->controls.push_back(this);

    PUSH_CODE(Jmp);
    jmp_offset = code.size();
    PUSH_CODE(Nop);
}

void RethrowStmt::gen_code(Module *module, Code *_code) {
    Stmt::gen_code(module, _code);

    auto &code = _code->code;

    PUSH_CODE(Rethrow);
}

void UseStmt::gen_code(Module *module, Code *_code) {
    Stmt::gen_code(module, _code);

    // Load module
    auto modname_const = Str::New(modname);

    if (!modname_const) {
        throw CodeGenException("Cannot allocate memory", _code->filename,
                               fileline);
    }

    auto modname_offset = ADD_CONST(modname_const);

    PUSH_CODE(LoadModule);
    PUSH_CODE(modname_offset);

    if (asname.empty()) {
        // Merge module
        PUSH_CODE(MergeModule);
    } else {
        // Alias
        auto asname_const = Str::New(asname);

        if (!asname_const) {
            throw CodeGenException("Cannot allocate memory", _code->filename,
                                   fileline);
        }

        auto asname_offset = ADD_CONST(asname_const);

        PUSH_CODE(StoreVar);
        PUSH_CODE(asname_offset);
        PUSH_CODE(Pop);
    }
}

void NewTypeStmt::gen_code(Module *module, Code *_code) {
    Stmt::gen_code(module, _code);

    // Push constructor if declared (push null otherwise)
    if (constructor) {
        constructor->gen_code(module, _code);
    } else {
        // Load null
        PUSH_CODE(LoadConst);
        auto null_off = ADD_CONST(null);
        PUSH_CODE(null_off);
    }

    auto name_const = Str::New(name);

    if (!name_const) {
        throw CodeGenException("Cannot allocate memory", _code->filename,
                               fileline);
    }

    auto name_offset = ADD_CONST(name_const);

    // Create
    PUSH_CODE(NewType);
    PUSH_CODE(name_offset);

    // Save it to a variable
    PUSH_CODE(StoreVar);
    PUSH_CODE(name_offset);
    PUSH_CODE(Pop);
}

void WhileStmt::gen_code(Module *module, Code *_code) {
    LoopStmt::gen_code(module, _code);

    _code->loop_stack.push_back(this);

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

    // Solve control statements
    for (auto ctrl : controls) {
        code[ctrl->jmp_offset] = ctrl->isbreak ? code.size() : start_offset;
    }

    _code->loop_stack.pop_back();
}

void ForStmt::gen_code(Module *module, Code *_code) {
    LoopStmt::gen_code(module, _code);

    _code->loop_stack.push_back(this);

    auto &code = _code->code;

    // The pseudo code of the generation
    // push iter
    // start:
    // push next iter element / goto finally if next item == enditer
    // body
    // goto start
    // finally:
    // pop iter

    // Push iter
    iterable->gen_code(module, _code);
    PUSH_CODE(UnaIter);

    // Generate condition
    auto start_offset = code.size();
    PUSH_CODE(ForNext);

    auto finally_offset = code.size();
    PUSH_CODE(Nop);

    // Store the next item within the id
    target->gen_code(module, _code);
    PUSH_CODE(Pop);

    // Execute body
    body->gen_code(module, _code);

    // Jump at the start section (loop again)
    PUSH_CODE(Jmp);
    PUSH_CODE(start_offset);

    // Finally section
    code[finally_offset] = code.size();

    // Solve control statements
    for (auto ctrl : controls) {
        code[ctrl->jmp_offset] = ctrl->isbreak ? code.size() : start_offset;
    }

    // Pop iter
    PUSH_CODE(Pop);

    _code->loop_stack.pop_back();
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
            auto const_id = Str::New(id);

            if (!const_id) {
                throw CodeGenException("Can't allocate memory",
                                       module->filepath, fileline);
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

    auto const_attr = Str::New(attr);

    if (!const_attr) {
        throw CodeGenException("Cannot allocate memory", module->filepath, fileline);
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
    Object *name = Str::New(id);

    if (!name) {
        throw CodeGenException("Can't allocate memory", module->filepath,
                               fileline);
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
        const_val = ::Str::New(get<str_t>(val));
        break;

    default:
        DEBUG_FATAL(module, fileline, "Const::gen_code : Unknown type");
    }

    if (!const_val) {
        throw CodeGenException("Not enough memory", module->filepath, fileline);
    }

    PUSH_CODE(LoadConst);

    auto const_offset = ADD_CONST(const_val);
    PUSH_CODE(const_offset);
}

// TODO B : Cascade
// TODO : --- This section is WIP ---
void Cascade::gen_code(Module *module, Code *_code) {
    this->debug();

    // TODO B : Multiple statements
    auto cascade_id = right->fetch_cascade_id();

    if (!cascade_id) {
        throw CodeGenException(
            "Invalid cascade expression, the right hand side expression must "
            "be compatible to an attribute expression",
            _code->filename, right->fileline);
    } else {
        cout << "OK" << endl;
    }

    auto c_id = *cascade_id;

    // TODO : Delete shallow
    *cascade_id =
        new Attr(c_id->fileline, left, reinterpret_cast<Id *&>(c_id)->id);

    // *cascade_id = new Attr(1, new Id(1, "o"), "a");

    // TODO : Handle frees
    auto old_left = left;
    left = new Id(0, "a");

    // left->debug();
    // right->debug();

    right->gen_code(module, _code);
    PUSH_CODE(Pop);

    old_left->gen_code(module, _code);

    // left->gen_code(module, _code);

    // PUSH_CODE(Dup);

    // right->gen_code(module, _code);
    // PUSH_CODE(Pop);
}

void RangeExp::gen_code(Module *module, Code *_code) {
    start->gen_code(module, _code);
    end->gen_code(module, _code);

    if (step)
        step->gen_code(module, _code);
    else {
        auto one = new (nothrow) Int(1);

        if (!one) {
            throw CodeGenException("Cannot allocate memory", _code->filename,
                                   fileline);
        }

        PUSH_CODE(LoadConst);
        auto one_offset = ADD_CONST(one);
        PUSH_CODE(one_offset);
    }

    PUSH_CODE(MakeRange);
    PUSH_CODE(static_cast<size_t>(inclusive));
}

// Generates the binary expression op code
void gen_binexp(Module *module, Code *_code, BinExp::Op op, int fileline) {
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

    case BinExp::NotEqual:
        PUSH_CODE(BinCmp);
        PUSH_CODE((opcode_t)CmpOp::NotEqual);
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

    case BinExp::Sub:
        PUSH_CODE(BinSub);
        break;

    case BinExp::Mul:
        PUSH_CODE(BinMul);
        break;

    case BinExp::Div:
        PUSH_CODE(BinDiv);
        break;

    case BinExp::Mod:
        PUSH_CODE(BinMod);
        break;

    case BinExp::Is:
        PUSH_CODE(BinIs);
        break;

    case BinExp::IsNot:
        PUSH_CODE(BinIs);
        PUSH_CODE(UnaNot);
        break;

    case BinExp::In:
        PUSH_CODE(BinIn);
        break;

    case BinExp::NotIn:
        PUSH_CODE(BinIn);
        PUSH_CODE(UnaNot);
        break;

    default:
        DEBUG_FATAL(module, fileline, "gen_binexp : Unknown op");
    }
}

void BinExp::gen_code(Module *module, Code *_code) {
    left->gen_code(module, _code);
    right->gen_code(module, _code);

    gen_binexp(module, _code, op, fileline);
}

void UnaExp::gen_code(Module *module, Code *_code) {
    exp->gen_code(module, _code);

    switch (op) {
    case UnaExp::Neg:
        PUSH_CODE(UnaNeg);
        break;

    case UnaExp::Not:
        PUSH_CODE(UnaNot);
        break;

    default:
        DEBUG_FATAL(module, fileline, "UnaExp::gen_code : Unknown op");
    }
}

void RelativeSet::gen_code(Module *module, Code *_code) {
    // Perform relative operation
    auto target_exp = target->get_exp();

    if (!target_exp)
        throw CodeGenException(
            "Cannot use relative assignment with this target", _code->filename,
            fileline);

    target_exp->gen_code(module, _code);

    exp->gen_code(module, _code);
    gen_binexp(module, _code, op, fileline);

    // Update value
    target->gen_code(module, _code);
}

// --- Targets ---
void MultiTarget::gen_code(Module *module, Code *_code) {
    // Unpack with length
    PUSH_CODE(Unpack);
    PUSH_CODE(targets.size());

    // Gen target + pop for each target
    for (int i = 0; i < targets.size(); ++i) {
        targets[targets.size() - i - 1]->gen_code(module, _code);

        PUSH_CODE(Pop);
    }
}

void IdTarget::gen_code(Module *module, Code *_code) {
    PUSH_CODE(StoreVar);

    auto const_id = Str::New(id->id);

    if (!const_id) {
        throw CodeGenException("Cannot allocate memory", module->filepath, fileline);
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
    auto const_attr = Str::New(attr->attr);

    if (!const_attr) {
        throw CodeGenException("Can't allocate memory", module->filepath,
                               fileline);
    }

    PUSH_CODE(StoreAttr);
    auto off_attr = ADD_CONST(const_attr);
    PUSH_CODE(off_attr);
}
