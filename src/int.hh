#pragma once

// The integer type

#include "object.hh"

struct Int : public Object {
    static Type *class_type;

    int_t data;

    // Can throw
    static void init_class_type();

    Int(const int_t &data);
};
