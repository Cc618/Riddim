#pragma once

// Built in functions

#include "object.hh"
#include "vec.hh"
#include "map.hh"

Object *print(Object *args, Object *kwargs);

Object *builtin_typeof(Object *args, Object *kwargs);

// Prints one object to stdout
inline Object *debug_print(Object *o) {
    return print(new Vec({o}), new HashMap());
}
