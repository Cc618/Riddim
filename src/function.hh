#pragma once

// Functor objects

#include "map.hh"
#include "object.hh"
#include "utils.hh"

struct Function : public Object {
    static Type *class_type;

    // null by default
    Object *self;

    // (Object *self, Vec *args, HashMap *kwargs) -> Object*
    fn_ternary_t data;

    // Can throw
    static void init_class_type();

    Function(const fn_ternary_t &data, Object *self = nullptr);
};
