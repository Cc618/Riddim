#include "builtins.hh"
#include "error.hh"
#include "function.hh"
#include "map.hh"
#include "null.hh"
#include "program.hh"
#include "str.hh"
#include "vec.hh"
#include <iostream>

using namespace std;

#define CHECK_ARGS(BUILTIN_NAME)                                               \
    if (args->type != Vec::class_type) {                                       \
        THROW_TYPE_ERROR_PREF(BUILTIN_NAME "{args}", args->type,               \
                              Vec::class_type);                                \
        return nullptr;                                                        \
    }

#define CHECK_KWARGS(BUILTIN_NAME)                                             \
    if (kwargs->type != HashMap::class_type) {                                 \
        THROW_TYPE_ERROR_PREF(BUILTIN_NAME "{kwargs}", args->type,             \
                              HashMap::class_type);                            \
        return nullptr;                                                        \
    }

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

void init_builtins() {
    auto &global_frame = Program::instance->global_frame;

#define INIT_BUILTIN(NAME, HANDLER)                                            \
    {                                                                          \
        auto obj = new (nothrow) Function(HANDLER);                            \
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

    INIT_BUILTIN("print", print);
    INIT_BUILTIN("typeof", builtin_typeof);

#undef INIT_BUILTIN
}

Object *print(Object *self, Object *args, Object *kwargs) {
    CHECK_ARGS("print");
    CHECK_KWARGS("print");

    auto args_data = reinterpret_cast<Vec *>(args)->data;
    auto kwargs_data = reinterpret_cast<HashMap *>(kwargs)->data;

    if (kwargs_data.size() != 0) {
        THROW_EXTRA_KWARGS("print", "No kwargs required");

        return nullptr;
    }

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
    CHECK_ARGS("typeof");
    CHECK_KWARGS("typeof");

    auto args_data = reinterpret_cast<Vec *>(args)->data;
    auto kwargs_data = reinterpret_cast<HashMap *>(kwargs)->data;

    if (kwargs_data.size() != 0) {
        THROW_EXTRA_KWARGS("typeof", "No kwargs required");

        return nullptr;
    }

    if (args_data.size() != 1) {
        THROW_ARGUMENT_ERROR("typeof", "length", "Only one argument required");

        return nullptr;
    }

    return args_data[0]->type;
}
