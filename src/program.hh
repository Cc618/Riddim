#pragma once

// Gathers all the data of a Riddim program at runtime
// It is the root of every object (we can access every object
// from it except unreferenced objects)

#include "error.hh"
#include "frame.hh"
#include "module.hh"
#include "object.hh"
#include <vector>

struct Program : public Object {
    static Program *instance;
    static Type *class_type;

    // This module must be added via add_module (will be garbage collected
    // otherwise)
    Module *main_module;
    std::vector<Object *> globals;
    Object *current_error;
    std::vector<Object *> obj_stack;

    // Not the main module frame but the builtins frame
    Frame *global_frame = nullptr;

    // The current frame executed
    Frame *top_frame = nullptr;

    // Factory, since it is a singleton, the result is Program::instance
    static void New(const std::vector<Type *> &types);

    static void init_class_type();

    // Registers a new type
    // Can throw
    static void add_type(Type *type);

    // Registers a new module
    static void add_module(Module *mod);

    // Init attributes that were impossible to init in New
    void init_attributes();

private:
    Program(const std::vector<Type *> &types);

private:
    // Registers this type to global_frame
    // !!! Doesn't update types but only global_frame, use add_type
    // Can throw
    void register_type(Type *type);

private:
    std::vector<Module *> modules;
    std::vector<Type *> types;
};
