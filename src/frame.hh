#pragma once

// Stack frame, contains variables within a scope

#include "map.hh"

struct Frame : public Object {
    static Type *class_type;

    Frame *previous;
    HashMap *vars;

    // Fetch the first variable named "name"
    // This is the main method to resolve variable / function names at runtime
    // Returns nullptr if not found with a name error (throws)
    // TODO lambda : Return frame where the variable belong to too (to save it)
    Object *fetch(Object *name);

    // Can throw
    static Frame *New(Frame *previous = nullptr);

    // Can throw
    static void init_class_type();

private:
    Frame(Frame *previous);
};
