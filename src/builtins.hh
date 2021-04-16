#pragma once

// Built in functions

#include "object.hh"
#include "vec.hh"
#include "map.hh"
#include "null.hh"

struct Program;

// Inits builtins and add them in the program's global frame
void init_builtins();

// --- Functions ---
Object *builtin_assert(Object *self, Object *args, Object *kwargs);

// Throws if is not Int
Object *builtin_hash(Object *self, Object *args, Object *kwargs);

Object *builtin_print(Object *self, Object *args, Object *kwargs);

Object *builtin_throw(Object *self, Object *args, Object *kwargs);

Object *builtin_typeof(Object *self, Object *args, Object *kwargs);

// --- Utils ---
// Prints one object to stdout
// !!! Use it only in debug mode since it is unsafe
inline Object *debug_print(Object *o) {
    return builtin_print(null, Vec::New({o}), HashMap::empty);
}
