#include "function.hh"
#include "error.hh"
#include "frame.hh"
#include "interpreter.hh"
#include "null.hh"
#include "program.hh"
#include "str.hh"
#include "vec.hh"
#include <unordered_set>

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

        for (const auto &[k, v] : me->args) {
            // Can be nullptr
            visit(v);
        }
    };

    // @call
    class_type->fn_call = [](Object *self, Object *args,
                             Object *kwargs) -> Object * {
        auto me = reinterpret_cast<CodeFunction *>(self);

        if (args->type != args_t::class_type) {
            THROW_TYPE_ERROR_PREF("CodeFunction.@call{args}", args->type,
                                  args_t::class_type);

            return nullptr;
        }

        auto posargs = reinterpret_cast<Vec *>(args)->data;

        if (posargs.size() > me->args.size()) {
            // TODO A : Function name
            if (me->n_required_args == me->args.size()) {
                THROW_ARGUMENT_ERROR("???", "length (too many arguments)",
                                     "This function requires " +
                                         to_string(me->args.size()) +
                                         " arguments");
            } else {
                THROW_ARGUMENT_ERROR("???", "length (too many arguments)",
                                     "This function requires from " +
                                         to_string(me->n_required_args) +
                                         " to " + to_string(me->args.size()) +
                                         " arguments");
            }

            return nullptr;
        }

        if (kwargs != null && kwargs->type != kwargs_t::class_type) {
            THROW_TYPE_ERROR_PREF("CodeFunction.@call{kwargs}", kwargs->type,
                                  kwargs_t::class_type);

            return nullptr;
        }

        auto kwargs_data = reinterpret_cast<HashMap *>(kwargs)->data;

        // Bind positional arguments
        std::unordered_map<str_t, Object *> vars;
        for (size_t i = 0; i < posargs.size(); ++i) {
            auto &arg_name = me->args[i].first;
            vars[arg_name] = posargs[i];
        }

        // Bind keyword arguments
        for (const auto &[h, kv] : kwargs_data) {
            const auto &[k, v] = kv;

            if (k->type != Str::class_type) {
                THROW_TYPE_ERROR_PREF("CodeFunction.@call", k->type, Str::class_type);

                return nullptr;
            }

            str_t key = reinterpret_cast<Str *>(k)->data;

            // This argument is already set
            if (vars.find(key) != vars.end()) {
                // TODO A : Func name
                THROW_ARGUMENT_ERROR("<unknown func>", key, "This argument has been set multiple times");

                return nullptr;
            }

            // Check whether this argument exists
            bool exists = false;
            for (const auto &[name, _] : me->args) {
                if (name == key) {
                    exists = true;
                    break;
                }
            }

            // Throw error
            if (!exists) {
                // TODO A : Func name
                THROW_ARGUMENT_ERROR("<unknown func>", key, "This argument doesn't exist");

                return nullptr;
            }

            // Set
            vars[key] = v;
        }

        // Evaluate default arguments
        for (const auto &[arg_name, arg_default] : me->args) {
            // This argument is not set
            if (vars.find(arg_name) == vars.end()) {
                if (!arg_default) {
                    // TODO A : Func name
                    THROW_ARGUMENT_ERROR("<unknown func>", arg_name, "Argument not set but required");

                    return nullptr;
                }

                // Evaluate the default value
                size_t ip = 0;
                interpret_fragment(arg_default, ip);

                // Evaluation error
                if (on_error()) {
                    return nullptr;
                }

                // Retrieve value and set it
                auto default_value = Program::instance->obj_stack.back();
                Program::instance->obj_stack.pop_back();
                vars[arg_name] = default_value;
            }
        }

        interpret(me->code, vars);

        if (on_error())
            return nullptr;

        // The return value is the TOS
        auto ret = Program::instance->obj_stack.back();
        Program::instance->obj_stack.pop_back();

        return ret;
    };
}
