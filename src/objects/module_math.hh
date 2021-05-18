#pragma once

#include "module.hh"
#include "builtins.hh"
#include "methods.hh"

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

// Returns the float_t y as a new Float
#define RETURN_Y                                                               \
    auto result = new (nothrow) Float(y);                                      \
    if (!result) {                                                             \
        THROW_MEMORY_ERROR;                                                    \
        return nullptr;                                                        \
    }                                                                          \
    return result;

// When math is loaded
void on_math_loaded(Module *mod);

BUILTIN_HANDLER(math, sqrt);
