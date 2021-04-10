#include "builtins.hh"
#include "error.hh"
#include "function.hh"
#include "map.hh"
#include "methods.hh"
#include "null.hh"
#include "program.hh"
#include "str.hh"
#include "vec.hh"
#include <iostream>

using namespace std;

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

#define INIT_CONSTRUCTOR(TYPE) INIT_BUILTIN(#TYPE, new_##TYPE)

    // Functions
    INIT_BUILTIN("print", print);
    INIT_BUILTIN("typeof", builtin_typeof);

    // Constructors
    // TODO : A constructor is a method within the Type (Int is a type, Int() is
    // a function call)
    INIT_CONSTRUCTOR(AttrObject);

#undef INIT_CONSTRUCTOR
#undef INIT_BUILTIN
}

// --- Functions ---
// Print
Object *print(Object *self, Object *args, Object *kwargs) {
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

// --- Constructors ---
Object *new_AttrObject(Object *self, Object *args, Object *kwargs) {
    INIT_METHOD(Object, "AttrObject");

    CHECK_NOARGS("AttrObject");
    CHECK_NOKWARGS("AttrObject");

    auto result = AttrObject::New();

    // Dispatch error
    if (!result)
        return nullptr;

    return result;
}
