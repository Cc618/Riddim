#pragma once

// String type
// TODO : Unicode

#include "object.hh"

struct Str : public Object {
    static Type *class_type;

    str_t data;

    // Can throw
    static void init_class_type();

    Str(const str_t &data);
};
