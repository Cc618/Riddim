#pragma once

// Map objects, they are equivalent to dicts in Python

#include "object.hh"
#include <map>

// An unordered map that provides a mapping in O(1) average time
// (up to O(N))
struct HashMap : public Object {
    static Type *class_type;

    std::unordered_map<Object *, Object *> data;

    HashMap();

    // TODO : As type lambda ?
    // Can throw
    Object *fn_index(Object *key);

    // Can throw
    static void init_class_type();
};
