#include "interpreter.hh"
#include "bool.hh"
#include "builtins.hh"
#include "debug.hh"
#include "error.hh"
#include "map.hh"
#include "program.hh"

// TODO
#include <iostream>

using namespace std;
using namespace OpCode;

// Prints the stack
// TODO : Rm
void debug_stack(const vector<Object*> &obj_stack) {
    cout << "Stack (" << obj_stack.size() << ") :" << endl;
    int staki = obj_stack.size();
    for (auto o : obj_stack) {
        cout << --staki << ". "
             << (o ? reinterpret_cast<Str *>(o->str())->data : "nullptr")
             << endl;
    }
}

// Pops the top of the stack and returns its value
inline Object *pop_top() {
    auto top = Program::instance->obj_stack.back();
    Program::instance->obj_stack.pop_back();

    return top;
}

void interpret(Code *_code, const std::unordered_map<str_t, Object*> &vars) {
#define NEXT(NARGS)                                                            \
    ip += (NARGS) + 1;                                                         \
    continue;
#define JMP(OFFSET)                                                            \
    ip = (OFFSET);                                                             \
    continue
#define ARG(N) code[ip + (N)]
#define TOP obj_stack.back();
#define PUSH(OBJ) obj_stack.push_back(OBJ);
// TODO : Trace
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

    // Push new frame
    auto frame = Frame::New(Program::instance->top_frame);
    for (const auto &[id, val] : vars) {
        auto o_id = new (nothrow) Str(id);
        if (!o_id) return;

        if (!frame->setitem(o_id, val)) return;
    }

    if (!frame) return;

    Program::instance->top_frame = frame;

    auto &ip = _code->ip;
    auto &code = _code->code;
    auto &consts = _code->consts->data;
    auto &obj_stack = Program::instance->obj_stack;

    ip = 0;

    while (42) {
        if (on_error()) {
            DISPATCH_ERROR;
        }

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

        case Call: {
            CHECK_STACKLEN(2);

            POPTOP(args);
            POPTOP(o);

            auto result = o->call(args, HashMap::empty);

            if (!result) {
                DISPATCH_ERROR;
            }

            PUSH(result);

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

            NEXT(0);
        }

        case Dup: {
            CHECK_STACKLEN(1);

            obj_stack.push_back(obj_stack.back());

            NEXT(0);
        }

        case Jmp: {
            auto offset = ARG(1);

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

            PUSH(newtos);

            NEXT(0);
        }

        case LoadVar: {
            auto name_off = ARG(1);
            CHECK_CONST(name_off);

            auto name = GET_CONST(name_off);

            if (!name) {
                DISPATCH_ERROR;
            }

            PUSH(frame->fetch(name));

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

        case Return: {
            cout << "RET" << endl;
            // Dispatch return
            Program::instance->top_frame = Program::instance->top_frame->previous;
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

        case UnaTypeOf: {
            CHECK_STACKLEN(1);

            POPTOP(tos);

            PUSH(tos->type);

            NEXT(0);
        }

        // TODO : Rm
        case DebugStack: {
            debug_stack(obj_stack);

            NEXT(0);
        }
        }

        // Error, ip outside of code (to exit, the opcode must be Exit or
        // Return)
        // TODO : Line info
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
    // TODO : Restore stack
    debug_info("IP : " + to_string(ip) +
               ", line of error : " + to_string(_code->lineof(ip)));

    return;
}
