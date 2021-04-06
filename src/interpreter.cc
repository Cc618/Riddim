#include "interpreter.hh"
#include "bool.hh"
#include "debug.hh"
#include "error.hh"
#include "program.hh"

// TODO
#include <iostream>

using namespace std;
using namespace OpCode;

// Pops the top of the stack and returns its value
inline Object *pop_top() {
    auto top = Program::instance->obj_stack.back();
    Program::instance->obj_stack.pop_back();

    return top;
}

void interpret(Frame *frame) {
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

// TODO : Restore stack
#define DISPATCH_ERROR return;

// Check stack length and throw on error
#define CHECK_STACKLEN(LEN)                                                    \
    if (obj_stack.size() < (LEN)) {                                            \
        THROW_STACK_TOOSMALL(LEN);                                             \
        DISPATCH_ERROR;                                                        \
    }

    auto &ip = frame->ip;
    auto &code = frame->code;
    auto &consts = frame->consts->data;
    auto &obj_stack = Program::instance->obj_stack;

    ip = 0;

    while (42) {
        if (on_error()) {
            DISPATCH_ERROR;
        }

        auto instruction = frame->code[ip];
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

        case Pop: {
            CHECK_STACKLEN(1);
            obj_stack.pop_back();
            NEXT(0);
        }

        case Return: {
            // Dispatch return
            return;
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

            POPTOP(val);

            frame->setitem(name, val);

            NEXT(1);
        }

        case TerCall: {
            CHECK_STACKLEN(3);

            POPTOP(tos);
            POPTOP(tos1);
            POPTOP(tos2);

            auto result = tos2->call(tos1, tos);

            if (!result) {
                DISPATCH_ERROR;
            }

            PUSH(result);

            NEXT(0);
        }

        // TODO : Rm
        case DebugStack: {
            cout << "Stack (" << obj_stack.size() << ") :" << endl;
            int staki = obj_stack.size();
            for (auto o : obj_stack) {
                cout << --staki << ". "
                     << (o ? reinterpret_cast<Str *>(o->str())->data
                           : "nullptr")
                     << endl;
            }

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
}
