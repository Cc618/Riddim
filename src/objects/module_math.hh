#pragma once

#include "module.hh"
#include "builtins.hh"
#include "methods.hh"

#define BUILTIN_HANDLER(MODULE, NAME)                                          \
    Object *builtin_##MODULE##_##NAME(Object *self, Object *args,              \
                                      Object *kwargs)

// When math is loaded
void on_math_loaded(Module *mod);

BUILTIN_HANDLER(math, sqrt);
