#pragma once

// String type
// TODO : Unicode

#include "object.hh"
#include "methods.hh"
#include <unordered_map>

struct Str : public DynamicObject {
    static DynamicType *class_type;

    str_t data;

    static Str *New(str_t data = "");

    // Can throw
    static void init_class_type();

    // Methods
    // Returns the index of the first occurence of a string
    // Returns -1 if not found
    DECL_METHOD(index);

    // Returns the length of the string
    DECL_METHOD(len);

protected:
    Str(const str_t &data);
};
