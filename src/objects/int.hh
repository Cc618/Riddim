#pragma once

// The integer type

#include "object.hh"

// Returns the index of the ith element in a collection of size len
// The index can be negative, in this case it will be modulated (-1 == last element)
// * Does not throw but can returns an index greater than the length of the collection
// * it can returns also a negative index in case of invalid negative index
int_t get_mod_index(int_t i, int_t len);

struct Int : public Object {
    static Type *class_type;

    static Int *zero;
    static Int *one;

    int_t data;

    // Can throw
    static void init_class_type();

    static void init_class_objects();

    Int(const int_t &data);
};
