#pragma once

// Vector type, an array where objects are stored contiguously

#include "object.hh"
#include "function.hh"
#include <vector>

// Internal Vec
typedef std::vector<Object*> vec_t;

struct Vec : public Object {
    static Type *class_type;
    static Vec *empty;

    vec_t data;

    // Methods
    Function *me_add;

    Vec(const vec_t &data = {});

    // Can throw
    static void init_class_type();
    static void init_class_objects();

    // Handler
    // Add an object
    static Object *me_add_handler(Object *self, Object *args, Object *kwargs);
};
