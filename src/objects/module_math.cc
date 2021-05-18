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

void on_math_loaded(Module *mod) {
    auto &global_frame = Program::instance->global_frame;

    const str_t sqrt_doc = "Returns the name of type\n\n"
                           "- type : Target\n"
                           "- return, Str : Name of type";

    // Signatures
    const builtin_signature_t sqrt_sig = {{"exp", false}, {"msg", true}};

    FAST_INIT_MODULE_BUILTIN(math, sqrt);
}

BUILTIN_HANDLER(math, sqrt) {
    // x is the argument
    INIT_X_METHOD("sqrt");

    float_t y = sqrt(x);

    RETURN_Y;
}
