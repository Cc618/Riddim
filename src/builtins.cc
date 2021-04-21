#include "builtins.hh"
#include "error.hh"
#include "frame.hh"
#include "function.hh"
#include "int.hh"
#include "map.hh"
#include "methods.hh"
#include "null.hh"
#include "program.hh"
#include "str.hh"
#include "bool.hh"
#include "vec.hh"
#include <iostream>

using namespace std;

// --- Static ---
static Object *print_object(Object *o) {
    if (!o) {
        cout << "nullptr";
        return null;
    }

    auto result = o->str();

    // Error
    if (!result) {
        return nullptr;
    }

    if (result->type != Str::class_type) {
        THROW_TYPE_ERROR_PREF("print_object", result->type, Str::class_type);

        return nullptr;
    }

    cout << reinterpret_cast<Str *>(result)->data;

    return null;
}

// --- Global Object ---
Type *Global::class_type = nullptr;

Global::Global() : Object(Global::class_type) {}

void Global::init_class_type() {
    class_type = new (nothrow) Type("Global");

    if (!class_type) {
        THROW_MEMORY_ERROR;

        return;
    }
}

// --- Globals ---
Global *enditer = nullptr;

// --- Init ---
void init_builtins() {
    auto &global_frame = Program::instance->global_frame;

#define INIT_BUILTIN(NAME, HANDLER)                                            \
    {                                                                          \
        auto obj = new (nothrow) Builtin(HANDLER, NAME);                       \
        if (!obj) {                                                            \
            THROW_MEMORY_ERROR;                                                \
            return;                                                            \
        }                                                                      \
        auto name = new (nothrow) Str(NAME);                                   \
        if (!name) {                                                           \
            THROW_MEMORY_ERROR;                                                \
            return;                                                            \
        }                                                                      \
        if (!global_frame->setitem(name, obj)) {                               \
            return;                                                            \
        }                                                                      \
    }

    // Functions
    INIT_BUILTIN("assert", builtin_assert);
    INIT_BUILTIN("hash", builtin_hash);
    INIT_BUILTIN("iter", builtin_iter);
    INIT_BUILTIN("len", builtin_len);
    INIT_BUILTIN("next", builtin_next);
    INIT_BUILTIN("print", builtin_print);
    INIT_BUILTIN("throw", builtin_throw);
    INIT_BUILTIN("typeof", builtin_typeof);

    // Globals
    enditer = new (nothrow) Global();
    if (!enditer) return;
    auto enditer_str = new (nothrow) Str("enditer");
    if (!enditer_str) return;
    Program::instance->global_frame->setitem(enditer_str, enditer);

#undef INIT_BUILTIN
}

// --- Functions ---
Object *builtin_assert(Object *self, Object *args, Object *kwargs) {
    INIT_METHOD(Object, "assert");

    CHECK_NOKWARGS("assert");

    if (args_data.size() < 1 || args_data.size() > 2) {
        THROW_ARGUMENT_ERROR("assert", "length", "1 or 2 arguments required");

        return nullptr;
    }

    auto condition = args_data[0];

    if (!condition)
        return nullptr;

    if (condition->type != Bool::class_type) {
        THROW_TYPE_ERROR_PREF("assert", condition->type, Bool::class_type);

        return nullptr;
    }

    auto msg = args_data.size() == 2 ? args_data[1] : nullptr;

    if (msg && msg->type != Str::class_type) {
        THROW_TYPE_ERROR_PREF("assert{msg}", msg->type, Str::class_type);

        return nullptr;
    }

    if (!reinterpret_cast<Bool *>(condition)->data) {
        str_t msg_str = msg ? " : " C_GREEN + reinterpret_cast<Str*>(msg)->data + C_NORMAL : "";

        throw_fmt(AssertError, "Assert failed%s", msg_str.c_str());

        return nullptr;
    }

    return null;
}

Object *builtin_hash(Object *self, Object *args, Object *kwargs) {
    INIT_METHOD(Object, "hash");

    CHECK_NOKWARGS("hash");
    CHECK_ARGSLEN(1, "hash");

    auto o = args_data[0];
    auto result = o->hash();

    if (!result)
        return nullptr;

    if (result->type != Int::class_type) {
        THROW_TYPE_ERROR_PREF(o->type->name + ".@hash", result->type,
                              Int::class_type);

        return nullptr;
    }

    return result;
}

Object *builtin_iter(Object *self, Object *args, Object *kwargs) {
    INIT_METHOD(Object, "iter");

    CHECK_ARGSLEN(1, "iter");
    CHECK_NOKWARGS("iter");

    return args_data[0]->iter();
}

Object *builtin_len(Object *self, Object *args, Object *kwargs) {
    INIT_METHOD(Object, "len");

    CHECK_NOKWARGS("len");
    CHECK_ARGSLEN(1, "len");

    auto o = args_data[0];
    auto result = o->len();

    if (!result)
        return nullptr;

    if (result->type != Int::class_type) {
        THROW_TYPE_ERROR_PREF(o->type->name + ".@len", result->type,
                              Int::class_type);

        return nullptr;
    }

    return result;
}

Object *builtin_next(Object *self, Object *args, Object *kwargs) {
    INIT_METHOD(Object, "next");

    CHECK_ARGSLEN(1, "next");
    CHECK_NOKWARGS("next");

    return args_data[0]->next();
}

// Print
Object *builtin_print(Object *self, Object *args, Object *kwargs) {
    INIT_METHOD(Object, "print");

    CHECK_NOKWARGS("print");

    if (!args_data.empty()) {
        // Dispatch errors
        if (!print_object(args_data[0]))
            return nullptr;

        for (size_t i = 1; i < args_data.size(); ++i) {
            cout << ' ';

            // Dispatch errors
            if (!print_object(args_data[i]))
                return nullptr;
        }
    }

    cout << endl;

    return null;
}

Object *builtin_typeof(Object *self, Object *args, Object *kwargs) {
    INIT_METHOD(Object, "typeof");

    CHECK_ARGSLEN(1, "typeof");
    CHECK_NOKWARGS("typeof");

    return args_data[0]->type;
}

Object *builtin_throw(Object *self, Object *args, Object *kwargs) {
    INIT_METHOD(Object, "throw");

    CHECK_ARGSLEN(1, "throw");
    CHECK_NOKWARGS("throw");

    throw_error(args_data[0]);

    return nullptr;
}

