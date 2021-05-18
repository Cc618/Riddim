#include "module_math.hh"
#include "bool.hh"
#include "error.hh"
#include "float.hh"
#include "function.hh"
#include "int.hh"
#include "null.hh"
#include "program.hh"

// Avoid typedef conflicts
#define float_t cmath_float_t
#include <cmath>
#undef float_t

using namespace std;

#define INIT_BUILTIN(NAME, HANDLER, DOC, SIGNATURE)                            \
    auto HANDLER##_obj =                                                \
        new (nothrow) Builtin(HANDLER, NAME, nullptr, DOC, SIGNATURE);         \
    if (!HANDLER##_obj) {                                               \
        THROW_MEMORY_ERROR;                                                    \
        return;                                                                \
    }                                                                          \
    auto HANDLER##_name = Str::New(NAME);                               \
    if (!HANDLER##_name) {                                              \
        return;                                                                \
    }                                                                          \
    if (!global_frame->setitem(HANDLER##_name, HANDLER##_obj)) { \
        return;                                                                \
    }

#define FAST_INIT_BUILTIN(MODULE, NAME)                                                \
    INIT_BUILTIN(#NAME, builtin_##MODULE##_##NAME, NAME##_doc, NAME##_sig);

void on_math_loaded(Module *mod) {
    auto &global_frame = Program::instance->global_frame;

    const str_t sqrt_doc = "Returns the name of type\n\n"
                               "- type : Target\n"
                               "- return, Str : Name of type";

    // Signatures
    const builtin_signature_t sqrt_sig = {{"exp", false}, {"msg", true}};

    // INIT_BUILTIN("sqrt", builtin_sqrt, sqrt_doc, sqrt_sig);
    FAST_INIT_BUILTIN(math, sqrt);
}

BUILTIN_HANDLER(math, sqrt) {
    INIT_METHOD(Object, "sqrt");

    CHECK_ARGSLEN(1, "sqrt");

    // Parse arg
    auto arg = args_data[0];
    float_t x = to_float("sqrt", arg);
    if (on_error()) {
        return nullptr;
    }

    float_t y = sqrt(x);

    auto result = new (nothrow) Float(y);
    if (!result) {
        THROW_MEMORY_ERROR;

        return nullptr;
    }

    return result;
}
