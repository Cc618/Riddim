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
    INIT_BUILTIN("hash", builtin_hash);
    INIT_BUILTIN("print", builtin_print);
    INIT_BUILTIN("typeof", builtin_typeof);

#undef INIT_BUILTIN
}

// --- Functions ---
Object *builtin_hash(Object *self, Object *args, Object *kwargs) {
    INIT_METHOD(Object, "hash");

    CHECK_NOKWARGS("hash");
    CHECK_ARGSLEN(1, "typeof");

    auto o = reinterpret_cast<Vec *>(args)->data[0];
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
