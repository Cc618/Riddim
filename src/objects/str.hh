#pragma once

// String type
// TODO : Unicode

#include "object.hh"
#include "methods.hh"
#include <unordered_map>

struct Str : public Object {
    static AttrType *class_type;

    str_t data;

    std::unordered_map<str_t, Object*> attrs;

    // Can throw
    static void init_class_type();

    Str(const str_t &data);

    // Methods
    // Returns the index of the first occurence of a string
    // Returns -1 if not found
    DECL_METHOD(index);

    // Returns the length of the string
    DECL_METHOD(len);
};
