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
#include <numeric>
#undef float_t

using namespace std;

void on_math_loaded(Module *mod) {
    auto &global_frame = Program::instance->global_frame;

    // Docs
    const str_t acos_doc = "Returns the arc cosinus of x";
    const str_t asin_doc = "Returns the arc sinus of x";
    const str_t atan_doc = "Returns the arc tangent of x";
    const str_t cos_doc = "Returns the cosinus of x";
    const str_t exp_doc = "Returns the exponential of x";
    const str_t factorial_doc = "Returns the factorial of Int(x)";
    const str_t gcd_doc = "Returns the greatest common divisor between a and b";
    const str_t isfinite_doc = "Returns whether x is neither inf nor nan";
    const str_t lcm_doc = "Returns the least common multiple between a and b";
    const str_t log2_doc = "Returns the logarithm (base 2) of x";
    const str_t log_doc = "Returns the natural logarithm of x";
    const str_t pow_doc = "Returns a power b";
    const str_t sin_doc = "Returns the sinus of x";
    const str_t sqrt_doc = "Returns the square root of x";
    const str_t tan_doc = "Returns the tangent of x";

    // Signatures
    const builtin_signature_t acos_sig = {{"x", false}};
    const builtin_signature_t asin_sig = {{"x", false}};
    const builtin_signature_t atan_sig = {{"x", false}};
    const builtin_signature_t cos_sig = {{"x", false}};
    const builtin_signature_t exp_sig = {{"x", false}};
    const builtin_signature_t factorial_sig = {{"x", false}};
    const builtin_signature_t gcd_sig = {{"a", false}, {"b", false}};
    const builtin_signature_t isfinite_sig = {{"x", false}};
    const builtin_signature_t lcm_sig = {{"a", false}, {"b", false}};
    const builtin_signature_t log2_sig = {{"x", false}};
    const builtin_signature_t log_sig = {{"x", false}};
    const builtin_signature_t pow_sig = {{"a", false}, {"b", false}};
    const builtin_signature_t sin_sig = {{"x", false}};
    const builtin_signature_t sqrt_sig = {{"x", false}};
    const builtin_signature_t tan_sig = {{"x", false}};

    // Init
    FAST_INIT_BUILTIN(math, acos);
    FAST_INIT_BUILTIN(math, asin);
    FAST_INIT_BUILTIN(math, atan);
    FAST_INIT_BUILTIN(math, cos);
    FAST_INIT_BUILTIN(math, exp);
    FAST_INIT_BUILTIN(math, factorial);
    FAST_INIT_BUILTIN(math, gcd);
    FAST_INIT_BUILTIN(math, isfinite);
    FAST_INIT_BUILTIN(math, lcm);
    FAST_INIT_BUILTIN(math, log);
    FAST_INIT_BUILTIN(math, log2);
    FAST_INIT_BUILTIN(math, pow);
    FAST_INIT_BUILTIN(math, sin);
    FAST_INIT_BUILTIN(math, sqrt);
    FAST_INIT_BUILTIN(math, tan);
}

BUILTIN_HANDLER(math, acos) {
    // x is the argument
    INIT_X_METHOD("acos");

    CHECK_WITHINEQ("acos", x, -1, 1);

    TRY_CMATH_FUN(acos);
}

BUILTIN_HANDLER(math, asin) {
    // x is the argument
    INIT_X_METHOD("asin");

    CHECK_WITHINEQ("asin", x, -1, 1);

    TRY_CMATH_FUN(asin);
}

BUILTIN_HANDLER(math, atan) {
    // x is the argument
    INIT_X_METHOD("atan");

    TRY_CMATH_FUN(atan);
}

BUILTIN_HANDLER(math, cos) {
    // x is the argument
    INIT_X_METHOD("cos");

    TRY_CMATH_FUN(cos);
}

BUILTIN_HANDLER(math, exp) {
    // x is the argument
    INIT_X_METHOD("exp");

    TRY_CMATH_FUN(exp);
}

BUILTIN_HANDLER(math, factorial) {
    // x is the argument
    INIT_X_METHOD("factorial");

    CHECK_GREATEREQ("factorial", x, 0);

    float_t y = 1;
    for (int_t i = 1; i <= (int_t)x; ++i) {
        y *= i;
    }

    RETURN_Y;
}

BUILTIN_HANDLER(math, gcd) {
    // Init a, b
    INIT_METHOD(Object, "gcd");
    CHECK_ARGSLEN(2, "gcd");
    CHECK_NOKWARGS("gcd");

    auto a_arg = args_data[0];
    if (a_arg->type != Int::class_type) {
        THROW_TYPE_ERROR_PREF("gcd", a_arg->type, Int::class_type);
        return nullptr;
    }
    int_t a = reinterpret_cast<Int*>(a_arg)->data;

    auto b_arg = args_data[1];
    if (b_arg->type != Int::class_type) {
        THROW_TYPE_ERROR_PREF("gcd", b_arg->type, Int::class_type);
        return nullptr;
    }
    int_t b = reinterpret_cast<Int*>(b_arg)->data;

    int_t y = gcd(a, b);

    RETURN_INIT(Int);
}

BUILTIN_HANDLER(math, isfinite) {
    // x is the argument
    INIT_X_METHOD("isfinite");

    bool y = isfinite(x);

    return y ? istrue : isfalse;
}

BUILTIN_HANDLER(math, lcm) {
    // Init a, b
    INIT_METHOD(Object, "lcm");
    CHECK_ARGSLEN(2, "lcm");
    CHECK_NOKWARGS("lcm");

    auto a_arg = args_data[0];
    if (a_arg->type != Int::class_type) {
        THROW_TYPE_ERROR_PREF("lcm", a_arg->type, Int::class_type);
        return nullptr;
    }
    int_t a = reinterpret_cast<Int*>(a_arg)->data;

    auto b_arg = args_data[1];
    if (b_arg->type != Int::class_type) {
        THROW_TYPE_ERROR_PREF("lcm", b_arg->type, Int::class_type);
        return nullptr;
    }
    int_t b = reinterpret_cast<Int*>(b_arg)->data;

    int_t y = lcm(a, b);

    RETURN_INIT(Int);
}

BUILTIN_HANDLER(math, log) {
    // x is the argument
    INIT_X_METHOD("log");

    CHECK_GREATER("log", x, 0);

    TRY_CMATH_FUN(log);
}

BUILTIN_HANDLER(math, log2) {
    // x is the argument
    INIT_X_METHOD("log2");

    CHECK_GREATER("log2", x, 0);

    TRY_CMATH_FUN(log2);
}

BUILTIN_HANDLER(math, pow) {
    INIT_AB_METHOD("pow");

    float_t y = pow(a, b);

    RETURN_Y;
}

BUILTIN_HANDLER(math, sin) {
    // x is the argument
    INIT_X_METHOD("sin");

    TRY_CMATH_FUN(sin);
}

BUILTIN_HANDLER(math, sqrt) {
    // x is the argument
    INIT_X_METHOD("sqrt");

    CHECK_GREATEREQ("sqrt", x, 0);

    TRY_CMATH_FUN(sqrt);
}

BUILTIN_HANDLER(math, tan) {
    // x is the argument
    INIT_X_METHOD("tan");

    TRY_CMATH_FUN(tan);
}
