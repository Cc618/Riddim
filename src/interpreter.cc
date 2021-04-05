#include "interpreter.hh"
#include "debug.hh"
#include "error.hh"
#include "program.hh"

// TODO
#include <iostream>

using namespace std;
using namespace OpCode;

void interpret(Frame *frame) {
#define NEXT(NARGS)                                                            \
    ip += (NARGS) + 1;                                                         \
    continue;
#define ARG(N) code[ip + (N)]
#define PUSH(OBJ) obj_stack.push_back(OBJ);
#define POP() obj_stack.pop_back();
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
            NEXT(1);
        }

        case Return: {
            auto val_off = ARG(1);
            CHECK_CONST(val_off);

            auto val = GET_CONST(val_off);

            if (!val) {
                DISPATCH_ERROR;
            }

            PUSH(val);

            return;
        }

        case DebugStack: {
            cout << "Stack (" << obj_stack.size() << ") :" << endl;
            for (auto o : obj_stack) {
                cout << (o ? reinterpret_cast<Str*>(o->str())->data : "nullptr") << endl;
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
