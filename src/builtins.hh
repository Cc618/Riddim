#pragma once

// Built in functions

#include "object.hh"
#include "vec.hh"
#include "map.hh"
#include "null.hh"

struct Program;

// Inits builtins and add them in the program's global frame
void init_builtins();

Object *print(Object *self, Object *args, Object *kwargs);

Object *builtin_typeof(Object *self, Object *args, Object *kwargs);

// Prints one object to stdout
inline Object *debug_print(Object *o) {
    return print(null, new Vec({o}), new HashMap());
}
