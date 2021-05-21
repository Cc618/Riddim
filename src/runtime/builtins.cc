#include "builtins.hh"
#include "bool.hh"
#include "error.hh"
#include "float.hh"
#include "frame.hh"
#include "function.hh"
#include "int.hh"
#include "map.hh"
#include "methods.hh"
#include "module_math.hh"
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

// Returns the indices of the min / the max and their value
// Can throw
// amin, amax, min, max
static tuple<int_t, int_t, Object *, Object *> argminmax_wrapper(Object *col) {
    auto iter = col->iter();
    if (!iter) {
        return {0, 0, nullptr, nullptr};
    }

    Object *current = iter->next();
    Object *min_object = current;
    Object *max_object = current;

    int_t max_index = 0;
    int_t min_index = 0;
    int_t index = 0;

    // Empty
    if (current == enditer) {
        throw_fmt(IndexError, "Min / max function with empty iterator");

        return {0, 0, nullptr, nullptr};
    }

    while (true) {
        if (!current) {
            return {0, 0, nullptr, nullptr};
        }

        if (current == enditer) {
            break;
        }

        // min <=> current
        auto min_threeway = min_object->cmp(current);

        if (!min_threeway) {
            return {0, 0, nullptr, nullptr};
        }

        // min > current
        if (reinterpret_cast<Int *>(min_threeway)->data > 0) {
            min_object = current;
            min_index = index;
        }

        // max <=> current
        auto max_threeway = max_object->cmp(current);

        if (!max_threeway) {
            return {0, 0, nullptr, nullptr};
        }

        // max < current
        if (reinterpret_cast<Int *>(max_threeway)->data < 0) {
            max_object = current;
            max_index = index;
        }

        current = iter->next();
        ++index;
    }

    return {min_index, max_index, min_object, max_object};
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

    // Docs
    const str_t abs_doc = "Returns the absolute value of x";

    const str_t argmin_doc =
        "Returns the index pointing at the minimum of col\n\n"
        "- col : Collection\n"
        "- return, Int : Index of the minimum";

    const str_t argmax_doc =
        "Returns the index pointing at the maximum of col\n\n"
        "- col : Collection\n"
        "- return, Int : Index of the maximum";

    const str_t argminmax_doc =
        "Returns the index pointing at the minimum and the maximum of col\n\n"
        "- col : Collection\n"
        "- return, Vec{Int} : Indices of the minimum and the maximum";

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

    const str_t min_doc = "Returns the minimum value within col\n\n"
                          "- col : Collection\n"
                          "- return : Minimum";

    const str_t max_doc = "Returns the maximum value within col\n\n"
                          "- col : Collection\n"
                          "- return : Maximum";

    const str_t minmax_doc =
        "Returns the minimum and the maximum values within col\n\n"
        "- col : Collection\n"
        "- return, Vec : Minimum and maximum values";

    const str_t next_doc =
        "Returns the next element of iterator (or enditer)\n\n"
        "- iterator : Object with @next slot\n"
        "- return : Next object or enditer";

    const str_t print_doc = "Prints to stdout all arguments\n\n"
                            "- [args...] : Objects to print";

    const str_t sort_doc = "Sorts the collection inplace\n\n"
                           "- col : Random access collection\n"
                           "- [cmp] : Custom compare, returns a <=> b, by default a@cmp is used\n"
                           "- return : Returns col";

    const str_t throw_doc = "Throws error\n\n"
                            "- error : Target error";

    const str_t typeof_doc = "Returns the type of obj\n\n"
                             "- obj : Target\n"
                             "- return, Type : Type of the object";

    const str_t typename_doc = "Returns the name of type\n\n"
                               "- type : Target\n"
                               "- return, Str : Name of type";

    // Signatures
    const builtin_signature_t abs_sig = {{"x", false}};
    const builtin_signature_t argmax_sig = {{"col", false}};
    const builtin_signature_t argmin_sig = {{"col", false}};
    const builtin_signature_t argminmax_sig = {{"col", false}};
    const builtin_signature_t assert_sig = {{"exp", false}, {"msg", true}};
    const builtin_signature_t copy_sig = {{"obj", false}};
    const builtin_signature_t doc_sig = {{"obj", false}};
    const builtin_signature_t exit_sig = {{"code", true}};
    const builtin_signature_t hash_sig = {{"obj", false}};
    const builtin_signature_t iter_sig = {{"iterable", false}};
    const builtin_signature_t len_sig = {{"col", false}};
    const builtin_signature_t max_sig = {{"col", false}};
    const builtin_signature_t min_sig = {{"col", false}};
    const builtin_signature_t minmax_sig = {{"col", false}};
    const builtin_signature_t next_sig = {{"iterator", false}};
    const builtin_signature_t print_sig = {{"args...", true}};
    const builtin_signature_t sort_sig = {{"col", false}};
    const builtin_signature_t throw_sig = {{"error", false}};
    const builtin_signature_t typename_sig = {{"type", false}};
    const builtin_signature_t typeof_sig = {{"obj", false}, {"cmp", true}};

    // Functions
    FAST_INIT_SINGLE_BUILTIN(abs);
    FAST_INIT_SINGLE_BUILTIN(argmax);
    FAST_INIT_SINGLE_BUILTIN(argmin);
    FAST_INIT_SINGLE_BUILTIN(argminmax);
    FAST_INIT_SINGLE_BUILTIN(assert);
    FAST_INIT_SINGLE_BUILTIN(copy);
    FAST_INIT_SINGLE_BUILTIN(doc);
    FAST_INIT_SINGLE_BUILTIN(exit);
    FAST_INIT_SINGLE_BUILTIN(hash);
    FAST_INIT_SINGLE_BUILTIN(iter);
    FAST_INIT_SINGLE_BUILTIN(len);
    FAST_INIT_SINGLE_BUILTIN(max);
    FAST_INIT_SINGLE_BUILTIN(min);
    FAST_INIT_SINGLE_BUILTIN(minmax);
    FAST_INIT_SINGLE_BUILTIN(next);
    FAST_INIT_SINGLE_BUILTIN(print);
    FAST_INIT_SINGLE_BUILTIN(sort);
    FAST_INIT_SINGLE_BUILTIN(throw);
    FAST_INIT_SINGLE_BUILTIN(typeof);
    FAST_INIT_SINGLE_BUILTIN(typename);

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
    REGISTER_GLOBAL(inf, float_inf);
    REGISTER_GLOBAL(nan, float_nan);

#undef REGISTER_GLOBAL

    // Modules
    Program::instance->builtin_modules["math"] = on_math_loaded;
}

// --- Functions ---
BUILTIN_HANDLER(builtins, abs) {
    INIT_METHOD(Object, "abs");

    CHECK_ARGSLEN(1, "abs");
    CHECK_NOKWARGS("abs");

    auto x = args_data[0];

    if (x->type == Int::class_type) {
        auto result = new (nothrow) Int(abs(reinterpret_cast<Int *>(x)->data));

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    } else if (x->type == Float::class_type) {
        auto result =
            new (nothrow) Float(abs(reinterpret_cast<Float *>(x)->data));

        if (!result) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }

        return result;
    } else {
        throw_fmt(TypeError, "Expected numeric type, got %s%s%s", C_BLUE,
                  x->type->name.c_str(), C_NORMAL);

        return nullptr;
    }
}

BUILTIN_HANDLER(builtins, argmax) {
    INIT_METHOD(Object, "argmax");

    CHECK_ARGSLEN(1, "argmax");
    CHECK_NOKWARGS("argmax");

    auto col = args_data[0];

    // amin, amax, min, max
    auto vals = argminmax_wrapper(col);

    if (on_error()) {
        return nullptr;
    }

    auto result = new (nothrow) Int(get<1>(vals));

    if (!result) {
        THROW_MEMORY_ERROR;

        return nullptr;
    }

    return result;
}

BUILTIN_HANDLER(builtins, argmin) {
    INIT_METHOD(Object, "argmin");

    CHECK_ARGSLEN(1, "argmin");
    CHECK_NOKWARGS("argmin");

    auto col = args_data[0];

    // amin, amax, min, max
    auto vals = argminmax_wrapper(col);

    if (on_error()) {
        return nullptr;
    }

    auto result = new (nothrow) Int(get<0>(vals));

    if (!result) {
        THROW_MEMORY_ERROR;

        return nullptr;
    }

    return result;
}

BUILTIN_HANDLER(builtins, argminmax) {
    INIT_METHOD(Object, "argminmax");

    CHECK_ARGSLEN(1, "argminmax");
    CHECK_NOKWARGS("argminmax");

    auto col = args_data[0];

    // amin, amax, min, max
    auto vals = argminmax_wrapper(col);

    if (on_error()) {
        return nullptr;
    }

    auto amin = new (nothrow) Int(get<0>(vals));

    if (!amin) {
        THROW_MEMORY_ERROR;

        return nullptr;
    }

    auto amax = new (nothrow) Int(get<1>(vals));

    if (!amax) {
        THROW_MEMORY_ERROR;

        return nullptr;
    }

    auto result = Vec::New({amin, amax});

    if (!result) {
        return nullptr;
    }

    return result;
}

BUILTIN_HANDLER(builtins, assert) {
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

BUILTIN_HANDLER(builtins, copy) {
    INIT_METHOD(Object, "copy");

    CHECK_ARGSLEN(1, "copy");
    CHECK_NOKWARGS("copy");

    auto result = args_data[0]->copy();

    if (!result) {
        return nullptr;
    }

    return result;
}

BUILTIN_HANDLER(builtins, doc) {
    INIT_METHOD(Object, "doc");

    CHECK_ARGSLEN(1, "doc");
    CHECK_NOKWARGS("doc");

    return args_data[0]->doc();
}

BUILTIN_HANDLER(builtins, exit) {
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

BUILTIN_HANDLER(builtins, hash) {
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

BUILTIN_HANDLER(builtins, iter) {
    INIT_METHOD(Object, "iter");

    CHECK_ARGSLEN(1, "iter");
    CHECK_NOKWARGS("iter");

    return args_data[0]->iter();
}

BUILTIN_HANDLER(builtins, len) {
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

BUILTIN_HANDLER(builtins, max) {
    INIT_METHOD(Object, "max");

    CHECK_ARGSLEN(1, "max");
    CHECK_NOKWARGS("max");

    auto col = args_data[0];

    // amin, amax, min, max
    auto vals = argminmax_wrapper(col);

    if (on_error()) {
        return nullptr;
    }

    auto result = get<3>(vals);

    return result;
}

BUILTIN_HANDLER(builtins, min) {
    INIT_METHOD(Object, "min");

    CHECK_ARGSLEN(1, "min");
    CHECK_NOKWARGS("min");

    auto col = args_data[0];

    // amin, amax, min, max
    auto vals = argminmax_wrapper(col);

    if (on_error()) {
        return nullptr;
    }

    auto result = get<2>(vals);

    return result;
}

BUILTIN_HANDLER(builtins, minmax) {
    INIT_METHOD(Object, "minmax");

    CHECK_ARGSLEN(1, "minmax");
    CHECK_NOKWARGS("minmax");

    auto col = args_data[0];

    // amin, amax, min, max
    auto vals = argminmax_wrapper(col);

    if (on_error()) {
        return nullptr;
    }

    auto result = Vec::New({get<2>(vals), get<3>(vals)});

    if (!result) {
        return nullptr;
    }

    return result;
}

BUILTIN_HANDLER(builtins, next) {
    INIT_METHOD(Object, "next");

    CHECK_ARGSLEN(1, "next");
    CHECK_NOKWARGS("next");

    return args_data[0]->next();
}

// Print
BUILTIN_HANDLER(builtins, print) {
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

BUILTIN_HANDLER(builtins, throw) {
    INIT_METHOD(Object, "throw");

    CHECK_ARGSLEN(1, "throw");
    CHECK_NOKWARGS("throw");

    throw_error(args_data[0]);

    return nullptr;
}

BUILTIN_HANDLER(builtins, typeof) {
    INIT_METHOD(Object, "typeof");

    CHECK_ARGSLEN(1, "typeof");
    CHECK_NOKWARGS("typeof");

    return args_data[0]->type;
}

BUILTIN_HANDLER(builtins, typename) {
    INIT_METHOD(Object, "typename");

    CHECK_ARGSLEN(1, "typename");
    CHECK_NOKWARGS("typename");

    if (!is_type(args_data[0])) {
        THROW_TYPE_ERROR_PREF("typename", args_data[0]->type, Type::class_type);

        return nullptr;
    }

    auto sname = Str::New(reinterpret_cast<Type *>(args_data[0])->name);

    if (!sname) {
        return nullptr;
    }

    return sname;
}

BUILTIN_HANDLER(builtins, sort) {
    INIT_METHOD(Object, "sort");

    CHECK_NOKWARGS("sort");

    Object *custom_cmp = nullptr;

    if (args_data.size() == 2) {
        custom_cmp = args_data[1];
    } else if (args_data.size() != 1) {
        THROW_ARGUMENT_ERROR("sort", "length", "1 or 2 arguments required");

        return nullptr;
    }

    // TODO A : tmp_stack for ints etc.

    auto col = args_data[0];
    auto len = col->len();
    if (!len) {
        return nullptr;
    }

    // Int guaranted
    auto size = reinterpret_cast<Int *>(len)->data;

    // Generate indices
    vector<Int *> indices(size);
    for (int_t i = 0; i < size; ++i) {
        indices[i] = new Int(i);

        if (!indices[i]) {
            THROW_MEMORY_ERROR;

            return nullptr;
        }
    }

    // TODO A : Quick sort
    for (int_t i = 0; i < size; ++i) {
        for (int_t j = 1; j < size; ++j) {
            auto a = col->getitem(indices[j - 1]);
            if (!a) {
                return nullptr;
            }

            auto b = col->getitem(indices[j]);
            if (!b) {
                return nullptr;
            }

            // Custom compare
            Object *cmp;
            if (custom_cmp) {
                auto callargs = Vec::New({a, b});
                if (!callargs) {
                    return nullptr;
                }

                cmp = custom_cmp->call(callargs, HashMap::empty);
            } else {
                cmp = a->cmp(b);
            }

            if (!cmp) {
                return nullptr;
            }

            // a > b
            if (reinterpret_cast<Int *>(cmp)->data > 0) {
                // Swap
                if (!col->setitem(indices[j - 1], b)) {
                    return nullptr;
                }

                if (!col->setitem(indices[j], a)) {
                    return nullptr;
                }
            }
        }
    }

    return col;
}
