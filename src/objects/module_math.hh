#pragma once

#include "builtins.hh"
#include "methods.hh"
#include "module.hh"

// Inits a function with one argument : x
#define INIT_X_METHOD(NAME)                                                    \
    INIT_METHOD(Object, NAME);                                                 \
    CHECK_ARGSLEN(1, NAME);                                                    \
    CHECK_NOKWARGS(NAME);                                                      \
    auto arg = args_data[0];                                                   \
    float_t x = to_float(NAME, arg);                                           \
    if (on_error()) {                                                          \
        return nullptr;                                                        \
    }

// Like INIT_X_METHOD but with 2 args : a and b
#define INIT_AB_METHOD(NAME)                                                   \
    INIT_METHOD(Object, NAME);                                                 \
    CHECK_ARGSLEN(2, NAME);                                                    \
    CHECK_NOKWARGS(NAME);                                                      \
    auto a_arg = args_data[0];                                                 \
    float_t a = to_float(NAME, a_arg);                                         \
    if (on_error()) {                                                          \
        return nullptr;                                                        \
    }                                                                          \
    auto b_arg = args_data[1];                                                 \
    float_t b = to_float(NAME, b_arg);                                         \
    if (on_error()) {                                                          \
        return nullptr;                                                        \
    }

// Returns y as a new TYPE
#define RETURN_INIT(TYPE)                                                      \
    auto result = new (nothrow) TYPE(y);                                       \
    if (!result) {                                                             \
        THROW_MEMORY_ERROR;                                                    \
        return nullptr;                                                        \
    }                                                                          \
    return result;

// Returns y as a new Float
#define RETURN_Y RETURN_INIT(Float)

// Tries to apply the cmath defined function FUN
#define TRY_CMATH_FUN(FUN)                                                     \
    try {                                                                      \
        float_t y = FUN(x);                                                    \
        RETURN_Y;                                                              \
    } catch (...) {                                                            \
        THROW_DOMAIN_ERROR(                                                    \
            #FUN, (#FUN "(" + to_string(x) + ") is undefined").c_str());       \
        return nullptr;                                                        \
    }

#define CHECK_GREATEREQ(FUN, X, LIMIT)                                         \
    if (X < LIMIT) {                                                           \
        THROW_DOMAIN_ERROR(FUN, #X " < " #LIMIT);                              \
        return nullptr;                                                        \
    }

#define CHECK_GREATER(FUN, X, LIMIT)                                           \
    if (X <= LIMIT) {                                                          \
        THROW_DOMAIN_ERROR(FUN, #X " <= " #LIMIT);                             \
        return nullptr;                                                        \
    }

#define CHECK_LESSEREQ(FUN, X, LIMIT)                                          \
    if (X > LIMIT) {                                                           \
        THROW_DOMAIN_ERROR(FUN, #X " > " #LIMIT);                              \
        return nullptr;                                                        \
    }

#define CHECK_LESSER(FUN, X, LIMIT)                                            \
    if (X >= LIMIT) {                                                          \
        THROW_DOMAIN_ERROR(FUN, #X " >= " #LIMIT);                             \
        return nullptr;                                                        \
    }

// Within inclusive range
#define CHECK_WITHINEQ(FUN, X, LOW, HIGH)                                      \
    CHECK_GREATEREQ(FUN, X, LOW);                                              \
    CHECK_LESSEREQ(FUN, X, HIGH);

// When math is loaded
void on_math_loaded(Module *mod);

BUILTIN_HANDLER(math, abs);
BUILTIN_HANDLER(math, acos);
BUILTIN_HANDLER(math, asin);
BUILTIN_HANDLER(math, atan);
BUILTIN_HANDLER(math, cos);
BUILTIN_HANDLER(math, exp);
BUILTIN_HANDLER(math, factorial);
BUILTIN_HANDLER(math, gcd);
BUILTIN_HANDLER(math, isfinite);
BUILTIN_HANDLER(math, lcm);
BUILTIN_HANDLER(math, log);
BUILTIN_HANDLER(math, log2);
BUILTIN_HANDLER(math, pow);
BUILTIN_HANDLER(math, sin);
BUILTIN_HANDLER(math, sqrt);
BUILTIN_HANDLER(math, tan);