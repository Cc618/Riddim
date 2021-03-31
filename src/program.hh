#pragma once

// Gathers all the data of a Riddim program at runtime
// It is the root of every object (we can access every object
// from it except unreferenced objects)

#include "object.hh"
#include "error.hh"
#include <vector>

struct Program : public Object {
    static Program *instance;
    static Type *class_type;

    std::vector<Object *> globals;
    Object *current_error;

    Program(const std::vector<Type *> &types);

    static void init_class_type();

    // Registers a new type
    static void add_type(Type *type);

private:
    std::vector<Type *> types;
};
