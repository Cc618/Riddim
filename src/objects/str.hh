#pragma once

// String type
// TODO : Unicode

#include "object.hh"
#include "methods.hh"

struct Str : public Object {
    static Type *class_type;

    str_t data;

    // Can throw
    static void init_class_type();

    Str(const str_t &data);

    // Methods
    // Returns the index of the first occurence of a string
    // Returns -1 if not found
    DECL_METHOD(index);
};
