#pragma once

// Gathers all the data of a Riddim program at runtime
// It is the root of every object (we can access every object
// from it except unreferenced objects)

#include "object.hh"
#include "error.hh"
#include "module.hh"
#include <vector>

struct Program : public Object {
    static Program *instance;
    static Type *class_type;

    // This module must be added via add_module (will be garbage collected otherwise)
    Module *main_module;
    std::vector<Object *> globals;
    Object *current_error;
    std::vector<Object *> obj_stack;

    Program(const std::vector<Type *> &types);

    static void init_class_type();

    // Registers a new type
    static void add_type(Type *type);

    // Registers a new module
    static void add_module(Module *mod);

private:
    std::vector<Module *> modules;
    std::vector<Type *> types;
};
