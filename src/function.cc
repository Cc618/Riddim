#include "function.hh"
#include "error.hh"
#include "null.hh"
#include "str.hh"
#include "vec.hh"
#include "frame.hh"

using namespace std;

// --- AbstractFunction ---
AbstractFunction::AbstractFunction(Type *type, Object *self)
    : Object(type), self(self) {}

// --- Function ---
Type *Function::class_type = nullptr;

Function::Function(const fn_ternary_t &data, Object *self)
    : AbstractFunction(Function::class_type, self ? self : null), data(data) {}

void Function::init_class_type() {
    class_type = new (nothrow) Type("Function");

    if (!class_type) {
        THROW_MEMORY_ERROR;

        return;
    }

    // TODO : Mul = combination operator ? [(f * g)(x) == f(g(x))]

    // @call
    class_type->fn_call = [](Object *self, Object *args,
                             Object *kwargs) -> Object * {
        auto me = reinterpret_cast<Function *>(self);

        if (args->type != args_t::class_type) {
            THROW_TYPE_ERROR_PREF("Function.@call{args}", args->type,
                                  args_t::class_type);

            return nullptr;
        }

        if (kwargs != null && kwargs->type != kwargs_t::class_type) {
            THROW_TYPE_ERROR_PREF("Function.@call{kwargs}", kwargs->type,
                                  kwargs_t::class_type);

            return nullptr;
        }

        return me->data(me->self, args, kwargs);
    };
}

// --- CodeFunction ---
Type *CodeFunction::class_type = nullptr;

CodeFunction *CodeFunction::New(Frame *frame, Object *_self) {
    auto self = new (nothrow) CodeFunction(frame, _self);

    if (!self) {
        THROW_MEMORY_ERROR;

        return nullptr;
    }

    return self;
}

CodeFunction::CodeFunction(Frame *frame, Object *self)
    : AbstractFunction(CodeFunction::class_type, self ? self : null), frame(frame) {}

void CodeFunction::init_class_type() {
    // TODO : Rename to Function and Function to Builtin ?
    class_type = new (nothrow) Type("CodeFunction");

    if (!class_type) {
        THROW_MEMORY_ERROR;

        return;
    }

    // @call
    // TODO A
    class_type->fn_call = [](Object *self, Object *args,
                             Object *kwargs) -> Object * {
        // auto me = reinterpret_cast<CodeFunction *>(self);

        // if (args->type != args_t::class_type) {
        //     THROW_TYPE_ERROR_PREF("CodeFunction.@call{args}", args->type,
        //                           args_t::class_type);

        //     return nullptr;
        // }

        // if (kwargs != null && kwargs->type != kwargs_t::class_type) {
        //     THROW_TYPE_ERROR_PREF("CodeFunction.@call{kwargs}", kwargs->type,
        //                           kwargs_t::class_type);

        //     return nullptr;
        // }

        // return me->data(me->self, args, kwargs);

        return nullptr;
    };
}
