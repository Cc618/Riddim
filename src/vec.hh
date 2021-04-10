#pragma once

// Vector type, an array where objects are stored contiguously

#include "object.hh"
#include "function.hh"
#include "methods.hh"
#include <vector>

// Internal Vec
typedef std::vector<Object*> vec_t;

struct Vec : public Object {
    static Type *class_type;
    static Vec *empty;

    vec_t data;

    // Methods
    // Push back a new object
    DECL_METHOD(add);
    // Pop back
    DECL_METHOD(pop);

    static Vec *New(const vec_t &data = {});

    // Can throw
    static void init_class_type();
    static void init_class_objects();

private:
    Vec(const vec_t &data = {});
};
