#pragma once

// Vector type, an array where objects are stored contiguously

#include "object.hh"
#include "methods.hh"
#include <vector>

struct Builtin;

// Internal Vec
typedef std::vector<Object*> vec_t;

struct Vec : public DynamicObject {
    static DynamicType *class_type;
    static Vec *empty;

    vec_t data;

    // Methods
    // Push back a new object
    DECL_ATTR_METHOD(add);

    // Pop back
    DECL_ATTR_METHOD(pop);

    static Vec *New(const vec_t &data = {});

    // Can throw
    static void init_class_type();
    static void init_class_objects();

protected:
    Vec(const vec_t &data = {});

private:
    static size_t class_hash;
};
