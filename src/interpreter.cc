#include "interpreter.hh"
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
#define ARG(N) code[ip + (N)]
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
        return;                                                                \
    }

// TODO : Restore stack
#define DISPATCH_ERROR continue;

    auto &ip = frame->ip;
    auto &code = frame->code;
    auto &consts = frame->consts->data;
    auto &obj_stack = Program::instance->obj_stack;

    ip = 0;

    while (42) {
        auto instruction = frame->code[ip];
        switch (instruction) {
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

        case Return: {
            // Dispatch return
            return;
        }

        // TODO : Rm
        case DebugStack: {
            cout << "Stack (" << obj_stack.size() << ") :" << endl;
            for (auto o : obj_stack) {
                cout << (o ? reinterpret_cast<Str *>(o->str())->data
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
