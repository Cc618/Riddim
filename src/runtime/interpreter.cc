#include "interpreter.hh"
#include "bool.hh"
#include "builtins.hh"
#include "debug.hh"
#include "error.hh"
#include "function.hh"
#include "int.hh"
#include "map.hh"
#include "modio.hh"
#include "program.hh"
#include "range.hh"
#include "usertype.hh"
#include <iostream>

using namespace std;
using namespace OpCode;

// Pops the top of the stack and returns its value
inline Object *pop_top() {
    auto top = Program::instance->obj_stack.back();
    Program::instance->obj_stack.pop_back();

    return top;
}

#define NEXT(NARGS)                                                            \
    ip += (NARGS) + 1;                                                         \
    continue;
#define JMP(OFFSET)                                                            \
    ip = (OFFSET);                                                             \
    continue
#define ARG(N) code[ip + (N)]
#define TOP obj_stack.back();
#define PUSH(OBJ) obj_stack.push_back(OBJ);
#define POPTOP(VAR)                                                            \
    if (obj_stack.empty()) {                                                   \
        throw_fmt(InternalError, "Stack empty for instruction %d",             \
                  instruction);                                                \
        DISPATCH_ERROR;                                                        \
    }                                                                          \
    auto VAR = pop_top();
#define CHECK_CONST(OFF)                                                       \
    if ((OFF) >= consts.size()) {                                              \
        throw_fmt(InternalError,                                               \
                  "Invalid virtual machine code : Constant offset %d outside " \
                  "of bounds (%d constants)",                                  \
                  (OFF), consts.size());                                       \
        return;                                                                \
    }
// Don't forget to call CHECK_CONST before
// Can throw
#define GET_CONST(OFF) consts[OFF]->copy();
#define CHECK_MEMORY(OBJ)                                                      \
    if (!obj) {                                                                \
        THROW_MEMORY_ERROR;                                                    \
        DISPATCH_ERROR;                                                        \
    }

#define DISPATCH_ERROR goto error_thrown;

// Check stack length and throw on error
#define CHECK_STACKLEN(LEN)                                                    \
    if (obj_stack.size() < (LEN)) {                                            \
        THROW_STACK_TOOSMALL(LEN);                                             \
        DISPATCH_ERROR;                                                        \
    }

#define COPY_IF_POD(VAR)                                                       \
    if (is_pod_object(VAR)) {                                                  \
        (VAR) = (VAR)->copy();                                                 \
        if (!(VAR)) {                                                          \
            DISPATCH_ERROR;                                                    \
        }                                                                      \
    }

bool interpret_program(Module *main_module) {
    Program::instance->main_module = main_module;

    if (!Program::instance->builtins_module) {
        auto mod_builtins = load_module("builtins", "");

        if (!mod_builtins) {
            throw_fmt(ImportError, "Cannot find std module %sbuiltins%s",
                      C_BLUE, C_NORMAL);

            return false;
        }

        interpret(mod_builtins->code, mod_builtins->name->data, {},
                  mod_builtins);

        if (on_error()) {
            return false;
        }

        Program::instance->builtins_module = mod_builtins;
    }

    interpret(main_module->code, "Module<main>", {}, main_module);

    if (on_error()) {
        if (Program::instance->current_error->type == ExitError) {
            return false;
        }

        // Print stack trace
        if (Program::instance->trace)
            Program::instance->trace->dump();

        cerr << endl;
        dump_error();

        clear_error();

        cerr << endl << "Uncaught error, exiting" << endl;

        return false;
    }

    return true;
}

void interpret(Code *_code, const str_t &id,
               const std::unordered_map<str_t, Object *> &vars, Module *module,
               Frame *lambda_frame) {
    // Push new frame
    Frame *frame = nullptr;

    // Set mod variable
    if (module) {
        auto mod_str = Str::New("mod");
        if (!mod_str) {
            return;
        }

        Program::instance->global_frame->setitem(mod_str, module);

        frame = module->frame;
        frame->previous = Program::instance->top_frame;
    } else if (lambda_frame) {
        frame = lambda_frame;
        frame->previous = Program::instance->top_frame;
    } else {
        frame = Frame::New(id, _code->filename, Program::instance->top_frame);
    }

    if (!frame)
        return;

    for (const auto &[id, val] : vars) {
        auto o_id = Str::New(id);
        if (!o_id)
            return;

        if (!frame->setitem(o_id, val))
            return;
    }

    Program::push_frame(frame);

    // Recursion error
    if (on_error())
        return;

    // Merge builtins module if necessary
    if (Program::instance->builtins_module && module) {
        merge_frames(frame, Program::instance->builtins_module->frame);
    }

    interpret_fragment(_code, frame->ip);

    Program::pop_frame();

    if (module) {
        module->loaded = true;
    }
}

void interpret_fragment(Code *_code, size_t &ip) {
    auto frame = Program::instance->top_frame;

    auto &code = _code->code;
    auto &consts = _code->consts->data;
    auto &obj_stack = Program::instance->obj_stack;
    auto obj_stack_start_size = obj_stack.size();

    ip = 0;

    while (42) {
        if (on_error()) {
            DISPATCH_ERROR;
        }

        // To be able to loop again on thrown error
    main_loop:;
        auto instruction = code[ip];
        switch (instruction) {
        case BinAdd: {
            CHECK_STACKLEN(2);

            POPTOP(tos);
            POPTOP(tos1);

            auto sm = tos1->add(tos);

            if (!sm) {
                DISPATCH_ERROR;
            }

            PUSH(sm);

            NEXT(0);
        }

        case BinBool: {
            CHECK_STACKLEN(2);

            auto op = ARG(1);

            POPTOP(tos);
            POPTOP(tos1);

            auto result = bool_binop(tos1, tos, static_cast<BoolBinOp>(op));

            if (!result) {
                DISPATCH_ERROR;
            }

            PUSH(result);

            NEXT(1);
        }

        case BinCmp: {
            CHECK_STACKLEN(2);

            auto op = ARG(1);

            POPTOP(tos);
            POPTOP(tos1);

            auto result = compare(tos1, tos, static_cast<CmpOp>(op));

            if (!result) {
                DISPATCH_ERROR;
            }

            PUSH(result);

            NEXT(1);
        }

        case BindLambda: {
            CHECK_STACKLEN(1);

            auto fun = TOP;

            if (fun->type != Function::class_type) {
                THROW_TYPE_ERROR_PREF("BindLambda", fun->type,
                                      Function::class_type);

                DISPATCH_ERROR;
            }

            auto lambda = reinterpret_cast<Function *>(fun);
            lambda->lambda_frame =
                Frame::New(lambda->name, lambda->code->filename);

            if (!lambda->lambda_frame) {
                DISPATCH_ERROR;
            }

            // For each variable of the current frame, bind it to the lambda
            for (const auto &[h, kv] : frame->vars->data) {
                const auto &[k, v] = kv;

                if (k->type != Str::class_type) {
                    THROW_TYPE_ERROR_PREF("HashMap.@getitem", k->type,
                                          Str::class_type);

                    DISPATCH_ERROR;
                }

                if (!lambda->lambda_frame->setitem(k, v)) {
                    DISPATCH_ERROR;
                }
            }

            NEXT(0);
        }

        case BinDiv: {
            CHECK_STACKLEN(2);

            POPTOP(tos);
            POPTOP(tos1);

            auto result = tos1->div(tos);

            if (!result) {
                DISPATCH_ERROR;
            }

            PUSH(result);

            NEXT(0);
        }

        case BinIs: {
            CHECK_STACKLEN(2);

            POPTOP(tos);
            POPTOP(tos1);

            // Verify type
            if (tos->type != Type::class_type) {
                THROW_TYPE_ERROR_PREF("is", tos->type, Type::class_type);

                DISPATCH_ERROR;
            }

            PUSH(tos1->type == tos ? istrue : isfalse);

            NEXT(0);
        }

        case BinIn: {
            CHECK_STACKLEN(2);

            POPTOP(tos);
            POPTOP(tos1);

            auto result = tos->in(tos1);

            if (!result) {
                DISPATCH_ERROR;
            }

            PUSH(result);

            NEXT(0);
        }

        case BinMod: {
            CHECK_STACKLEN(2);

            POPTOP(tos);
            POPTOP(tos1);

            auto prod = tos1->mod(tos);

            if (!prod) {
                DISPATCH_ERROR;
            }

            PUSH(prod);

            NEXT(0);
        }

        case BinMul: {
            CHECK_STACKLEN(2);

            POPTOP(tos);
            POPTOP(tos1);

            auto prod = tos1->mul(tos);

            if (!prod) {
                DISPATCH_ERROR;
            }

            PUSH(prod);

            NEXT(0);
        }

        case BinSub: {
            CHECK_STACKLEN(2);

            POPTOP(tos);
            POPTOP(tos1);

            auto prod = tos1->sub(tos);

            if (!prod) {
                DISPATCH_ERROR;
            }

            PUSH(prod);

            NEXT(0);
        }

        case Call: {
            CHECK_STACKLEN(2);

            POPTOP(args);
            POPTOP(o);

            auto result = o->call(args, HashMap::empty);

            if (!result) {
                DISPATCH_ERROR;
            }

            PUSH(result);

            gc_step();

            NEXT(0);
        }

        case CallKw: {
            CHECK_STACKLEN(3);

            POPTOP(kwargs);
            POPTOP(args);
            POPTOP(o);

            auto result = o->call(args, kwargs);

            if (!result) {
                DISPATCH_ERROR;
            }

            PUSH(result);

            gc_step();

            NEXT(0);
        }

        case CallProc: {
            CHECK_STACKLEN(1);

            POPTOP(o);

            auto result = o->call(Vec::empty, HashMap::empty);

            if (!result) {
                DISPATCH_ERROR;
            }

            PUSH(result);

            gc_step();

            NEXT(0);
        }

        case CatchError: {
            CHECK_STACKLEN(1);

            // Can be null
            POPTOP(error_type);

            // Avoid exit error issues
            Program::instance->exit_code = 0;

            if (error_type != null && error_type->type != Type::class_type) {
                THROW_TYPE_ERROR_PREF("catch", error_type->type,
                                      Type::class_type);

                DISPATCH_ERROR;
            }

            auto error = Program::instance->caught_error;

            // Matches
            if (error_type == null || error->type == error_type) {
                auto id = GET_CONST(ARG(1));

                if (id != null) {
                    if (id->type != Str::class_type) {
                        throw_fmt(
                            InternalError,
                            "Opcode CatchError : Invalid catch id type '%s'",
                            id->type->name.c_str());

                        DISPATCH_ERROR;
                    }

                    // Set this variable to the error
                    frame->setitem(id, error);
                }

                clear_error();
            } else {
                auto nomatch_offset = ARG(2);

                // Don't clear error and jump to next instruction
                JMP(nomatch_offset);
            }

            NEXT(2);
        }

        case CatchTerminate: {
            Program::instance->caught_error = nullptr;

            auto offset = ARG(1);

            gc_step();

            JMP(offset);
        }

        case Dup: {
            CHECK_STACKLEN(1);

            obj_stack.push_back(obj_stack.back());

            NEXT(0);
        }

        case DupTos1: {
            CHECK_STACKLEN(2);

            obj_stack.push_back(obj_stack[obj_stack.size() - 2]);

            NEXT(0);
        }

        case ForNext: {
            CHECK_STACKLEN(1);

            auto tos = TOP;
            auto nextitem = tos->next();

            if (!nextitem) {
                DISPATCH_ERROR;
            }

            // End of loop, jump
            if (nextitem == enditer) {
                auto offset = ARG(1);

                gc_step();
                JMP(offset);
            }

            // Otherwise, push the next item
            PUSH(nextitem);

            NEXT(1);
        }

        case Jmp: {
            auto offset = ARG(1);

            gc_step();

            JMP(offset);
        }

        case JmpFalse: {
            CHECK_STACKLEN(1);

            auto offset = ARG(1);

            POPTOP(tos);

            if (tos->type != Bool::class_type) {
                THROW_TYPE_ERROR_PREF("Jmp", tos->type, Bool::class_type);

                DISPATCH_ERROR;
            }

            gc_step();

            if (reinterpret_cast<Bool *>(tos)->data) {
                NEXT(1);
            } else {
                JMP(offset);
            }
        }

        case LoadAttr: {
            CHECK_STACKLEN(1);

            // Load name
            auto name_off = ARG(1);
            CHECK_CONST(name_off);

            auto name = GET_CONST(name_off);

            if (!name) {
                DISPATCH_ERROR;
            }

            POPTOP(tos);

            // Load attribute
            auto result = tos->getattr(name);

            if (!result) {
                DISPATCH_ERROR;
            }

            COPY_IF_POD(result);

            PUSH(result);

            NEXT(1);
        }

        case LoadConst: {
            auto val_off = ARG(1);
            CHECK_CONST(val_off);

            auto val = GET_CONST(val_off);

            if (!val) {
                DISPATCH_ERROR;
            }

            PUSH(val);

            NEXT(1);
        }

        case LoadIndex: {
            CHECK_STACKLEN(2);

            POPTOP(tos);
            POPTOP(tos1);

            auto newtos = tos1->getitem(tos);

            if (!newtos) {
                DISPATCH_ERROR;
            }

            COPY_IF_POD(newtos);

            PUSH(newtos);

            NEXT(0);
        }

        case LoadModule: {
            // Load name
            auto name_off = ARG(1);
            CHECK_CONST(name_off);

            auto name = GET_CONST(name_off);

            if (!name) {
                DISPATCH_ERROR;
            }

            if (name->type != Str::class_type) {
                THROW_TYPE_ERROR_PREF("LoadModule", name->type,
                                      Str::class_type);

                DISPATCH_ERROR;
            }

            auto result = load_module(reinterpret_cast<Str *>(name)->data,
                                      _code->filename);

            if (!result) {
                DISPATCH_ERROR;
            }

            if (!result->loaded) {
                interpret(result->code, result->name->data, {}, result);

                if (on_error()) {
                    DISPATCH_ERROR;
                }

                // Pop the return value of the module
                POPTOP(_unused);
            }

            // Push the module
            PUSH(result);

            NEXT(1);
        }

        case LoadVar: {
            auto name_off = ARG(1);
            CHECK_CONST(name_off);

            auto name = GET_CONST(name_off);

            if (!name) {
                DISPATCH_ERROR;
            }

            auto result = frame->fetch(name);

            if (!result) {
                DISPATCH_ERROR;
            }

            COPY_IF_POD(result);

            PUSH(result);

            NEXT(1);
        }

        case MakeRange: {
            CHECK_STACKLEN(3);

            POPTOP(step);
            POPTOP(end);
            POPTOP(start);
            auto inclusive = ARG(1);

            if (start->type != Int::class_type) {
                THROW_TYPE_ERROR_PREF("Range operator (..), start argument",
                                      start->type, Int::class_type);

                DISPATCH_ERROR;
            }

            if (end->type != Int::class_type) {
                THROW_TYPE_ERROR_PREF("Range operator (..), end argument",
                                      end->type, Int::class_type);

                DISPATCH_ERROR;
            }

            if (step->type != Int::class_type) {
                THROW_TYPE_ERROR_PREF("Range operator (..), step argument",
                                      step->type, Int::class_type);

                DISPATCH_ERROR;
            }

            auto result =
                Range::New(reinterpret_cast<Int *>(start)->data,
                           reinterpret_cast<Int *>(end)->data,
                           reinterpret_cast<Int *>(step)->data, inclusive);

            if (!result) {
                DISPATCH_ERROR;
            }

            PUSH(result);

            NEXT(1);
        }

        case MergeModule: {
            CHECK_STACKLEN(1);

            POPTOP(tos);

            if (tos->type != Module::class_type) {
                THROW_TYPE_ERROR_PREF("MergeModule", tos->type,
                                      Module::class_type);

                DISPATCH_ERROR;
            }

            auto mod = reinterpret_cast<Module *>(tos);

            merge_frames(frame, mod->frame);

            NEXT(0);
        }

        case NewType: {
            CHECK_STACKLEN(1);

            POPTOP(constructor);

            auto name_offset = ARG(1);
            CHECK_CONST(name_offset);
            auto name_obj = GET_CONST(name_offset);

            if (name_obj->type != Str::class_type) {
                THROW_TYPE_ERROR_PREF("NewType", name_obj->type,
                                      Str::class_type);

                DISPATCH_ERROR;
            }

            str_t name = reinterpret_cast<Str *>(name_obj)->data;

            auto result = NewUserType(name);

            if (!result) {
                DISPATCH_ERROR;
            }

            if (constructor != null) {
                auto constructor_key = Str::New("@new");

                if (!constructor_key) {
                    DISPATCH_ERROR;
                }

                if (!result->setattr(constructor_key, constructor)) {
                    DISPATCH_ERROR;
                }
            }

            PUSH(result);

            NEXT(1);
        }

        case Nop: {
            NEXT(0);
        }

        case Pack: {
            auto count = ARG(1);

            CHECK_STACKLEN(count);

            // Pick objects
            vector<Object *> data(obj_stack.end() - count, obj_stack.end());
            obj_stack.resize(obj_stack.size() - count);

            // Copy PODs
            for (auto &arg : data) {
                COPY_IF_POD(arg);
            }

            // Build result
            auto result = Vec::New(data);

            if (!result) {
                DISPATCH_ERROR;
            }

            PUSH(result);

            NEXT(1);
        }

        case PackMap: {
            auto count = ARG(1);

            CHECK_STACKLEN(count * 2);

            // Build result
            auto result = HashMap::New();

            if (!result) {
                DISPATCH_ERROR;
            }

            // Pick objects
            for (size_t i = 0; i < count; ++i) {
                auto key = obj_stack[obj_stack.size() - count * 2 + i * 2];
                auto val = obj_stack[obj_stack.size() - count * 2 + i * 2 + 1];

                COPY_IF_POD(key);
                COPY_IF_POD(val);

                if (!result->setitem(key, val)) {
                    DISPATCH_ERROR;
                }
            }

            obj_stack.resize(obj_stack.size() - count * 2);

            PUSH(result);

            NEXT(1);
        }

        case Pop: {
            CHECK_STACKLEN(1);

            obj_stack.pop_back();

            NEXT(0);
        }

        case PopTryBlock: {
            frame->tryblocks.pop_back();

            NEXT(0);
        }

        case PushTryBlock: {
            auto offset = ARG(1);

            Frame::TryBlock block{offset, obj_stack.size()};
            frame->tryblocks.push_back(block);

            NEXT(1);
        }

        case Rethrow: {
            if (!Program::instance->caught_error) {
                throw_fmt(RuntimeError,
                          "%sRethrow%s outside of try-catch block", C_RED,
                          C_NORMAL);

                DISPATCH_ERROR;
            }

            Program::instance->current_error = Program::instance->caught_error;
            Program::instance->caught_error = nullptr;

            DISPATCH_ERROR;
        }

        case Return: {
            if (obj_stack.size() != obj_stack_start_size + 1) {
                throw_fmt(InternalError,
                          "Return : Invalid stack size, should be "
                          "obj_stack_start_size + 1 (%s%d + 1%s) but is %s%d%s",
                          C_GREEN, obj_stack_start_size, C_NORMAL, C_RED,
                          obj_stack.size(), C_NORMAL);

                DISPATCH_ERROR;
            }

            auto &tos = TOP;
            COPY_IF_POD(tos);

            gc_step();

            // Dispatch return
            return;
        }

        case StoreAttr: {
            CHECK_STACKLEN(2);

            // Load name
            auto name_off = ARG(1);
            CHECK_CONST(name_off);

            auto name = GET_CONST(name_off);

            if (!name) {
                DISPATCH_ERROR;
            }

            POPTOP(tos);
            auto tos1 = TOP;

            // Set attribute
            auto result = tos->setattr(name, tos1);

            if (!result) {
                DISPATCH_ERROR;
            }

            NEXT(1);
        }

        case StoreIndex: {
            CHECK_STACKLEN(3);

            POPTOP(tos);
            POPTOP(tos1);
            auto tos2 = TOP(obj_stack);

            tos1->setitem(tos, tos2);

            NEXT(0);
        }

        case StoreVar: {
            CHECK_STACKLEN(1);

            auto name_off = ARG(1);
            CHECK_CONST(name_off);

            auto name = GET_CONST(name_off);

            if (!name) {
                DISPATCH_ERROR;
            }

            if (name->type != Str::class_type) {
                THROW_TYPE_ERROR_PREF("interpret", name->type, Str::class_type);
                DISPATCH_ERROR;
            }

            auto val = TOP;

            frame->setitem(name, val);

            NEXT(1);
        }

        case UnaIter: {
            CHECK_STACKLEN(1);

            POPTOP(tos);

            auto result = tos->iter();

            if (!result) {
                DISPATCH_ERROR;
            }

            PUSH(result);

            NEXT(0);
        }

        case UnaNot: {
            CHECK_STACKLEN(1);

            POPTOP(tos);

            auto result = bool_not(tos);

            if (!result) {
                DISPATCH_ERROR;
            }

            PUSH(result);

            NEXT(0);
        }

        case UnaNeg: {
            CHECK_STACKLEN(1);

            POPTOP(tos);

            auto result = tos->neg();

            if (!result) {
                DISPATCH_ERROR;
            }

            PUSH(result);

            NEXT(0);
        }

        case UnaTypeOf: {
            CHECK_STACKLEN(1);

            POPTOP(tos);

            PUSH(tos->type);

            NEXT(0);
        }

        case Unpack: {
            CHECK_STACKLEN(1);

            auto tos = TOP;

            // Int verified
            auto toslen = tos->len();

            if (!toslen) {
                DISPATCH_ERROR;
            }

            size_t required_len = ARG(1);
            size_t current_len = reinterpret_cast<Int *>(toslen)->data;

            if (required_len != current_len) {
                throw_fmt(
                    IndexError,
                    "Cannot unpack %s%zu%s values, expected %s%zu%s values",
                    C_RED, current_len, C_NORMAL, C_BLUE, required_len,
                    C_NORMAL);

                DISPATCH_ERROR;
            }

            // Push each item
            for (size_t i = 0; i < current_len; ++i) {
                auto index = new (nothrow) Int(i);

                if (!index) {
                    THROW_MEMORY_ERROR;

                    DISPATCH_ERROR;
                }

                // Object to unpack
                auto val = tos->getitem(index);

                if (!val) {
                    DISPATCH_ERROR;
                }

                PUSH(val);
            }

            NEXT(1);
        }
        }

        // Error, ip outside of code (to exit, the opcode must be Exit or
        // Return)
        throw_fmt(InternalError,
                  "interpret: Instruction pointer (%d) outside of bounds "
                  "(length = %d)",
                  ip, code.size());
        return;
    }

    // Should not be executed (only Return and errors return)
    return;

// When an error is thrown within the switch
error_thrown:;
    if (!frame->tryblocks.empty()) {
        auto block = frame->tryblocks.back();
        frame->tryblocks.pop_back();

        // Restore state
        obj_stack.resize(block.stack_size);
        ip = block.catch_offset;
        Program::instance->caught_error = Program::instance->current_error;
        Program::instance->current_error = nullptr;

        goto main_loop;
    }

    auto trace = Trace::New(ip, _code, frame->id);

    // Do not throw again
    if (trace)
        Program::push_trace(trace);

    // Restore stack
    obj_stack.resize(obj_stack_start_size);

    return;
}
