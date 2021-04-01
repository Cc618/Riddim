#pragma once

// The null object

#include "object.hh"

struct Null;

// Instance, this is a singleton
extern Null *null;

struct Null : public Object {
    static Type *class_type;

    // Can throw
    static void init_class_type();

    // Inits the null object
    // Can throw
    static void init_singleton();

    Null();
};
