#pragma once

// Gathers all the data of a Riddim program at runtime
// It is the root of every object (we can access every object
// from it except unreferenced objects)

#include "error.hh"
#include "frame.hh"
#include "module.hh"
#include "object.hh"
#include "trace.hh"
#include <vector>
#include <sstream>

struct Program : public Object {
    static Program *instance;
    static Type *class_type;

    // Standard library modules path
    static str_t std_path;

    static bool types_loaded;
    static bool instances_loaded;

    str_t main_module_path;

    int_t exit_code = 0;

    // The current error description
    // Only used outside of the runtime (codegen + parser)
    std::stringstream errout;

    // Unreachable variables from Riddim
    std::vector<Object *> globals;
    std::vector<Object *> obj_stack;

    // This module must be added via add_module (will be garbage collected
    // otherwise)
    Module *main_module;

    Object *current_error;

    // Saved current_error in a catch block
    Object *caught_error;

    // Not the main module frame but the builtins frame
    Frame *global_frame = nullptr;

    // The current frame executed
    Frame *top_frame = nullptr;

    // The trace back (trace is the oldest)
    Trace *trace = nullptr;

    // Current call count
    size_t recursion_depth = 0;

    // Absolute path / module
    // Used to avoid reloading 2 times a module
    std::unordered_map<str_t, Module *> modules;

    // Factory, since it is a singleton, the result is Program::instance
    static void New(const std::vector<Type *> &types);

    static void init_class_type();

    // Registers a new type
    // Can throw
    static void add_type(Type *type);

    // Registers a new module
    static void add_module(Module *mod);

    // Adds a global variable unreachable from Riddim
    static void add_global(Object *o);

    // Updates the top of frame, links this frame with the
    // previous TOF
    // Can throw
    static void push_frame(Frame *f);
    static void pop_frame();

    // Pushes a new trace, this one is older than the current frame
    static void push_trace(Trace *t);
    static void pop_trace();

    // Outputs to cerr the errout stream
    // The stream is cleared
    static void output_errout();

    // Init attributes that were impossible to init in New
    // - filename : Main module file path
    void init_attributes();

private:
    Program(const std::vector<Type *> &types);

private:
    // Registers this type to global_frame
    // !!! Doesn't update types but only global_frame, use add_type
    // Can throw
    void register_type(Type *type);

private:
    std::vector<Type *> types;
};
