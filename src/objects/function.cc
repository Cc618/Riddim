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

static auto default_setattr(Object *self, Object *name, Object *val)
    -> Object * {
    auto me = dynamic_cast<AbstractFunction *>(self);

    if (!me) {
        throw_fmt(RuntimeError,
                  "AbstractFunction@setattr : Got an invalid AbstractFunction");

        return nullptr;
    }

    if (name->type != Str::class_type) {
        THROW_TYPE_ERROR_PREF("Function.@setattr", name->type, Str::class_type);

        return nullptr;
    }

    auto attr = reinterpret_cast<Str *>(name)->data;

    // Name
    if (attr == "!name") {
        if (val->type != Str::class_type) {
            THROW_TYPE_ERROR_PREF("Function.@setattr{!name}", val->type,
                                  Str::class_type);

            return nullptr;
        }

        me->name = reinterpret_cast<Str *>(val)->data;
    }
    // Doc
    else if (attr == "!doc") {
        if (val->type != Str::class_type) {
            THROW_TYPE_ERROR_PREF("Function.@setattr{!doc}", val->type,
                                  Str::class_type);

            return nullptr;
        }

        me->doc_str = reinterpret_cast<Str *>(val)->data;
    }

    return null;
}

// --- AbstractFunction ---
AbstractFunction::AbstractFunction(Type *type, Object *self, const str_t &name,
                                   const str_t &doc_str)
    : Object(type), self(self), name(name), doc_str(doc_str) {}

// --- Builtin ---
Type *Builtin::class_type = nullptr;

Builtin::Builtin(const fn_ternary_t &data, const str_t &name, Object *self,
                 const str_t &doc_str, const builtin_signature_t &doc_signature)
    : AbstractFunction(Builtin::class_type, self ? self : null, name, doc_str),
      data(data), doc_signature(doc_signature) {}

void Builtin::init_class_type() {
    class_type = new (nothrow) Type("Builtin");

    if (!class_type) {
        THROW_MEMORY_ERROR;

        return;
    }

    class_type->fn_traverse_objects = [](Object *self,
                                         const fn_visit_object_t &visit) {
        Builtin *me = reinterpret_cast<Builtin *>(self);

        visit(me->self);
    };

    // @call
    class_type->fn_call = [](Object *self, Object *args,
                             Object *kwargs) -> Object * {
        auto me = reinterpret_cast<Builtin *>(self);

        if (args->type != args_t::class_type) {
            THROW_TYPE_ERROR_PREF("Builtin.@call{args}", args->type,
                                  args_t::class_type);

            return nullptr;
        }

        if (kwargs != null && kwargs->type != kwargs_t::class_type) {
            THROW_TYPE_ERROR_PREF("Builtin.@call{kwargs}", kwargs->type,
                                  kwargs_t::class_type);

            return nullptr;
        }

        return me->data(me->self, args, kwargs);
    };

    // @copy
    class_type->fn_copy = [](Object *self) -> Object * {
        auto me = reinterpret_cast<Builtin *>(self);

        auto result =
            new (nothrow) Builtin(me->data, me->name, me->self, me->doc_str);

        if (!result) {
            return nullptr;
        }

        return result;
    };

    // @doc
    class_type->fn_doc = [](Object *self) -> Object * {
        auto me = reinterpret_cast<Builtin *>(self);

        // Create signature
        str_t signature = "> " + me->name + "(";
        if (!me->doc_signature.empty()) {
            // To avoid boilerplate code, appends the argument to signature
            auto add_arg = [&signature](const pair<str_t, bool> &arg) {
                // Optional
                if (arg.second)
                    signature += '[';

                signature += arg.first;

                // Optional
                if (arg.second)
                    signature += ']';
            };

            add_arg(me->doc_signature.front());

            for (int i = 1; i < me->doc_signature.size(); ++i) {
                signature += ", ";

                add_arg(me->doc_signature[i]);
            }
        }

        signature += ")";

        str_t doc_full = me->doc_str;

        if (!me->doc_str.empty())
            doc_full += "\n\n";

        doc_full += signature;

        auto doc = Str::New(doc_full);

        if (!doc)
            return nullptr;

        return doc;
    };

    // @setattr
    class_type->fn_setattr = default_setattr;

    // @str
    class_type->fn_str = [](Object *self) -> Object * {
        auto me = reinterpret_cast<Builtin *>(self);

        auto result = Str::New("Builtin<" + me->name + ">");

        if (!result) {
            return nullptr;
        }

        return result;
    };
}

// --- Function ---
Type *Function::class_type = nullptr;

Function *Function::New(Code *code, const str_t &name, Object *_self,
                        const str_t &doc_str) {
    auto self = new (nothrow) Function(code, name, _self, doc_str);

    if (!self) {
        THROW_MEMORY_ERROR;

        return nullptr;
    }

    return self;
}

Function::Function(Code *code, const str_t &name, Object *self,
                   const str_t &doc_str)
    : AbstractFunction(Function::class_type, self ? self : null, name, doc_str),
      code(code) {}

void Function::init_class_type() {
    class_type = new (nothrow) Type("Function");

    if (!class_type) {
        THROW_MEMORY_ERROR;

        return;
    }

    class_type->fn_traverse_objects = [](Object *self,
                                         const fn_visit_object_t &visit) {
        Function *me = reinterpret_cast<Function *>(self);

        visit(me->code);

        for (const auto &[k, v] : me->args) {
            // Can be nullptr
            visit(v);
        }

        visit(me->lambda_frame);
        visit(me->self);
    };

    // @doc
    class_type->fn_doc = [](Object *self) -> Object * {
        auto me = reinterpret_cast<Function *>(self);

        // Create signature
        str_t signature = "> " + me->name + "(";
        if (!me->args.empty()) {
            // To avoid boilerplate code, appends the argument to signature
            auto add_arg = [&signature](const pair<str_t, Code *> &arg) {
                // Optional
                if (arg.second)
                    signature += '[';

                signature += arg.first;

                // Optional
                if (arg.second)
                    signature += ']';
            };

            add_arg(me->args.front());

            for (int i = 1; i < me->args.size(); ++i) {
                signature += ", ";

                add_arg(me->args[i]);
            }
        }

        signature += ")";

        str_t doc_full = me->doc_str;

        if (!me->doc_str.empty())
            doc_full += "\n\n";

        doc_full += signature;

        auto doc = Str::New(doc_full);

        if (!doc)
            return nullptr;

        return doc;
    };

    // @call
    class_type->fn_call = [](Object *self, Object *args,
                             Object *kwargs) -> Object * {
        auto me = reinterpret_cast<Function *>(self);

        if (args->type != args_t::class_type) {
            THROW_TYPE_ERROR_PREF("Function<" + me->name + ">.@call{args}",
                                  args->type, args_t::class_type);

            return nullptr;
        }

        auto posargs = reinterpret_cast<Vec *>(args)->data;

        if (posargs.size() > me->args.size()) {
            if (me->n_required_args == me->args.size()) {
                THROW_ARGUMENT_ERROR(me->name, "length (too many arguments)",
                                     "This function requires " +
                                         to_string(me->args.size()) +
                                         " arguments");
            } else {
                THROW_ARGUMENT_ERROR(me->name, "length (too many arguments)",
                                     "This function requires from " +
                                         to_string(me->n_required_args) +
                                         " to " + to_string(me->args.size()) +
                                         " arguments");
            }

            return nullptr;
        }

        if (kwargs != null && kwargs->type != kwargs_t::class_type) {
            THROW_TYPE_ERROR_PREF("Function.@call{kwargs}", kwargs->type,
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
                THROW_TYPE_ERROR_PREF("Function.@call", k->type,
                                      Str::class_type);

                return nullptr;
            }

            str_t key = reinterpret_cast<Str *>(k)->data;

            // This argument is already set
            if (vars.find(key) != vars.end()) {
                THROW_ARGUMENT_ERROR(
                    me->name, key, "This argument has been set multiple times");

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
                THROW_ARGUMENT_ERROR(me->name, key,
                                     "This argument doesn't exist");

                return nullptr;
            }

            // Set
            vars[key] = v;
        }

        Object *ret = nullptr;

        auto old_tmp_stack_size = Program::instance->tmp_stack.size();

        Program::instance->tmp_stack.push_back(me);

        // Evaluate default arguments
        for (const auto &[arg_name, arg_default] : me->args) {
            // This argument is not set
            if (vars.find(arg_name) == vars.end()) {
                if (!arg_default) {
                    THROW_ARGUMENT_ERROR(me->name, arg_name,
                                         "Argument not set but required");

                    goto error;
                }

                // Evaluate the default value
                size_t ip = 0;
                interpret_fragment(arg_default, ip);

                // Evaluation error
                if (on_error()) {
                    goto error;
                }

                // Retrieve value and set it
                auto default_value = Program::instance->obj_stack.back();
                vars[arg_name] = default_value;

                // Avoid gc collect
                Program::instance->tmp_stack.push_back(default_value);
                Program::instance->obj_stack.pop_back();
            }
        }

        // Add the 'me' variable
        if (me->self)
            vars["me"] = me->self;

        interpret(me->code, "Function<" + me->name + ">", vars, nullptr,
                  me->lambda_frame);

        if (on_error()) {
            goto error;
        }

        // The return value is the TOS
        ret = Program::instance->obj_stack.back();
        Program::instance->obj_stack.pop_back();

        Program::instance->tmp_stack.resize(old_tmp_stack_size);

        return ret;

    error:;
        Program::instance->tmp_stack.resize(old_tmp_stack_size);

        return nullptr;
    };

    // @copy
    class_type->fn_copy = [](Object *self) -> Object * {
        auto me = reinterpret_cast<Function *>(self);

        auto result = Function::New(me->code, me->name, me->self, me->doc_str);

        if (!result) {
            return nullptr;
        }

        result->n_required_args = me->n_required_args;
        result->args = me->args;
        result->lambda_frame = me->lambda_frame;

        return result;
    };

    // @setattr
    class_type->fn_setattr = default_setattr;

    class_type->fn_str = [](Object *self) -> Object * {
        auto me = reinterpret_cast<Function *>(self);

        auto result = Str::New("Function<" + me->name + ">");

        if (!result) {
            return nullptr;
        }

        return result;
    };
}
