#include "function.hh"
#include "error.hh"
#include "frame.hh"
#include "interpreter.hh"
#include "null.hh"
#include "str.hh"
#include "vec.hh"
#include "program.hh"

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

CodeFunction *CodeFunction::New(Code *code, const str_t &name, Object *_self) {
    auto self = new (nothrow) CodeFunction(code, name, _self);

    if (!self) {
        THROW_MEMORY_ERROR;

        return nullptr;
    }

    return self;
}

CodeFunction::CodeFunction(Code *code, const str_t &name, Object *self)
    : AbstractFunction(CodeFunction::class_type, self ? self : null),
      code(code), name(name) {}

void CodeFunction::init_class_type() {
    // TODO B : Rename to Function and Function to Builtin ?
    class_type = new (nothrow) Type("CodeFunction");

    if (!class_type) {
        THROW_MEMORY_ERROR;

        return;
    }

    class_type->fn_traverse_objects = [](Object *self,
                                         const fn_visit_object_t &visit) {
        CodeFunction *me = reinterpret_cast<CodeFunction *>(self);

        visit(me->code);
    };

    // @call
    class_type->fn_call = [](Object *self, Object *args,
                             Object *kwargs) -> Object * {
        auto me = reinterpret_cast<CodeFunction *>(self);

        // TODO C : Handle args
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

        interpret(me->code);

        if (on_error()) return nullptr;

        // The return value is the TOS
        auto ret = Program::instance->obj_stack.back();
        Program::instance->obj_stack.pop_back();

        return ret;
    };
}
