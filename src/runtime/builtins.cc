#include "builtins.hh"
#include "bool.hh"
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
#include "module_math.hh"
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

#define INIT_BUILTIN(NAME, HANDLER, DOC, SIGNATURE)                            \
    {                                                                          \
        auto obj =                                                             \
            new (nothrow) Builtin(HANDLER, NAME, nullptr, DOC, SIGNATURE);     \
        if (!obj) {                                                            \
            THROW_MEMORY_ERROR;                                                \
            return;                                                            \
        }                                                                      \
        auto name = Str::New(NAME);                                            \
        if (!name) {                                                           \
            return;                                                            \
        }                                                                      \
        if (!global_frame->setitem(name, obj)) {                               \
            return;                                                            \
        }                                                                      \
    }

    // Docs
    const str_t assert_doc = "Throws AssertionError if exp is false\n\n"
                             "- exp, Bool : Expression to test\n"
                             "- [msg], String : Error message";

    const str_t copy_doc = "Shallow copies of obj\n\n"
                           "- obj : Object to copy\n"
                           "- return : Copied object";

    const str_t doc_doc = "Returns the documentation of obj\n\n"
                          "- obj : Object with documentation\n"
                          "- return, Str : Documentation";

    const str_t exit_doc = "Exits the program\n\n"
                           "- [code : 0], Int : Exit code";

    const str_t hash_doc = "Returns the hash value of obj\n\n"
                           "- obj : Target\n"
                           "- return, Int : Hash";

    const str_t iter_doc =
        "Returns the iterator of iterable (begin position)\n\n"
        "- iterable : Object with @iter slot\n"
        "- return : Iterator like object";

    const str_t len_doc = "Returns the length of col\n\n"
                          "- col : Object with @len slot\n"
                          "- return, Int : Length";

    const str_t next_doc =
        "Returns the next element of iterator (or enditer)\n\n"
        "- iterator : Object with @next slot\n"
        "- return : Next object or enditer";

    const str_t print_doc = "Prints to stdout all arguments\n\n"
                            "- [args...] : Objects to print";

    const str_t throw_doc = "Throws error\n\n"
                            "- error : Target error";

    const str_t typeof_doc = "Returns the type of obj\n\n"
                             "- obj : Target\n"
                             "- return, Type : Type of the object";

    const str_t typename_doc = "Returns the name of type\n\n"
                             "- type : Target\n"
                             "- return, Str : Name of type";

    // Signatures
    const builtin_signature_t assert_sig = {{"exp", false}, {"msg", true}};

    const builtin_signature_t copy_sig = {{"obj", false}};

    const builtin_signature_t doc_sig = {{"obj", false}};

    const builtin_signature_t exit_sig = {{"code", true}};

    const builtin_signature_t hash_sig = {{"obj", false}};

    const builtin_signature_t iter_sig = {{"iterable", false}};

    const builtin_signature_t len_sig = {{"col", false}};

    const builtin_signature_t next_sig = {{"iterator", false}};

    const builtin_signature_t print_sig = {{"args...", true}};

    const builtin_signature_t throw_sig = {{"error", false}};

    const builtin_signature_t typeof_sig = {{"obj", false}};

    const builtin_signature_t typename_sig = {{"type", false}};

    // Functions
#define FAST_INIT_BUILTIN(NAME)                                                \
    INIT_BUILTIN(#NAME, builtin_##NAME, NAME##_doc, NAME##_sig);

    FAST_INIT_BUILTIN(assert);
    FAST_INIT_BUILTIN(copy);
    FAST_INIT_BUILTIN(doc);
    FAST_INIT_BUILTIN(exit);
    FAST_INIT_BUILTIN(hash);
    FAST_INIT_BUILTIN(iter);
    FAST_INIT_BUILTIN(len);
    FAST_INIT_BUILTIN(next);
    FAST_INIT_BUILTIN(print);
    FAST_INIT_BUILTIN(throw);
    FAST_INIT_BUILTIN(typeof);
    FAST_INIT_BUILTIN(typename);

    // Globals
#define REGISTER_GLOBAL(NAME, ID)                                              \
    auto NAME##_str = Str::New(#NAME);                                         \
    if (!NAME##_str)                                                           \
        return;                                                                \
    Program::instance->global_frame->setitem(NAME##_str, ID);

    // enditer
    enditer = new (nothrow) Global();
    if (!enditer)
        return;
    REGISTER_GLOBAL(enditer, enditer);

    REGISTER_GLOBAL(true, istrue);
    REGISTER_GLOBAL(false, isfalse);
    REGISTER_GLOBAL(null, null);

#undef REGISTER_GLOBAL
#undef INIT_BUILTIN

    // Modules
    Program::instance->builtin_modules["math"] = on_math_loaded;
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
        str_t msg_str =
            msg ? " : " C_GREEN + reinterpret_cast<Str *>(msg)->data + C_NORMAL
                : "";

        throw_fmt(AssertError, "Assert failed%s", msg_str.c_str());

        return nullptr;
    }

    return null;
}

Object *builtin_copy(Object *self, Object *args, Object *kwargs) {
    INIT_METHOD(Object, "copy");

    CHECK_ARGSLEN(1, "copy");
    CHECK_NOKWARGS("copy");

    auto result = args_data[0]->copy();

    if (!result) {
        return nullptr;
    }

    return result;
}

Object *builtin_doc(Object *self, Object *args, Object *kwargs) {
    INIT_METHOD(Object, "doc");

    CHECK_ARGSLEN(1, "doc");
    CHECK_NOKWARGS("doc");

    return args_data[0]->doc();
}

Object *builtin_exit(Object *self, Object *args, Object *kwargs) {
    INIT_METHOD(Object, "exit");

    CHECK_NOKWARGS("exit");

    int_t code = 0;

    if (args_data.size() == 1) {
        auto arg = args_data.front();

        if (arg->type != Int::class_type) {
            THROW_TYPE_ERROR_PREF("exit", arg->type, Int::class_type);

            return nullptr;
        }

        code = reinterpret_cast<Int *>(arg)->data;
    } else if (!args_data.empty()) {
        THROW_ARGUMENT_ERROR("exit", "length", "0 or 1 arguments required");

        return nullptr;
    }

    Program::instance->exit_code = code;

    throw_fmt(ExitError, to_string(code).c_str());

    return nullptr;
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

Object *builtin_throw(Object *self, Object *args, Object *kwargs) {
    INIT_METHOD(Object, "throw");

    CHECK_ARGSLEN(1, "throw");
    CHECK_NOKWARGS("throw");

    throw_error(args_data[0]);

    return nullptr;
}

Object *builtin_typeof(Object *self, Object *args, Object *kwargs) {
    INIT_METHOD(Object, "typeof");

    CHECK_ARGSLEN(1, "typeof");
    CHECK_NOKWARGS("typeof");

    return args_data[0]->type;
}

Object *builtin_typename(Object *self, Object *args, Object *kwargs) {
    INIT_METHOD(Object, "typename");

    CHECK_ARGSLEN(1, "typename");
    CHECK_NOKWARGS("typename");

    if (!is_type(args_data[0])) {
        THROW_TYPE_ERROR_PREF("typename", args_data[0]->type, Type::class_type);

        return nullptr;
    }

    auto sname = Str::New(reinterpret_cast<Type*>(args_data[0])->name);

    if (!sname) {
        return nullptr;
    }

    return sname;
}
