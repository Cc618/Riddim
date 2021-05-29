#pragma once

// String type
// TODO : Unicode

#include "methods.hh"
#include "object.hh"
#include <unordered_map>

// Max value for a character
#define STR_CHAR_MAX 255

struct Str : public DynamicObject {
    static DynamicType *class_type;

    str_t data;

    static Str *New(str_t data = "");

    // Can throw
    static void init_class_type();

    // Can throw
    static void init_class_objects();

    // Methods
    DECL_METHOD(add);

    DECL_METHOD(chr);

    // Returns the index of the first occurence of a string
    // Returns -1 if not found
    DECL_METHOD(index);

    DECL_METHOD(lower);

    DECL_METHOD(ord);

    DECL_METHOD(upper);

protected:
    Str(const str_t &data);
};
