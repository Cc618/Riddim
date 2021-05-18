#pragma once

// The floating point type

#include "object.hh"

// Returns the value of the object
// The object can be an integer
// Throws if invalid type
float_t to_float(const str_t &fn_name, Object *o);

struct Float : public Object {
    static Type *class_type;

    static Object *zero;
    static Object *one;

    float_t data;

    // Can throw
    static void init_class_type();

    static void init_class_objects();

    Float(const float_t &data);
};

