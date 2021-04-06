#pragma once

// Functor objects

#include "object.hh"
#include "utils.hh"
#include "vec.hh"
#include "map.hh"

// TODO : Self
struct Function : public Object {
    static Type *class_type;

    // (Vec *args, HashMap *kwargs) -> Object*
    fn_ternary_t data;

    // Can throw
    static void init_class_type();

    Function(const fn_ternary_t &data);
};
