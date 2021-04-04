#pragma once

// Vector type, an array where objects are stored contiguously

#include "object.hh"
#include <vector>

// Internal Vec
typedef std::vector<Object*> vec_t;

struct Vec : public Object {
    static Type *class_type;

    vec_t data;

    Vec();

    // Can throw
    static void init_class_type();
};
