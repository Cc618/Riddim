#pragma once

// The floating point type

#include "object.hh"

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

