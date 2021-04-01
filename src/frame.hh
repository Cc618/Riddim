#pragma once

// Stack frame, contains variables within a scope

#include "map.hh"

struct Frame : public Object {
    static Type *class_type;

    Frame *previous;
    HashMap *vars;

    Frame();

    // TODO : fn_in

    // Can throw
    static void init_class_type();
};
