#pragma once

// Basic iterator type
// Note that we don't have to use this class to be iterable
// This class is just a convenient way to create an iterator

#include "object.hh"
#include <functional>

struct Iterator;

typedef std::function<Object *(Iterator *it)> fn_next_item_t;

struct Iterator : public Object {
    static Type *class_type;

    Object *collection;

    int_t custom_data;

    // See Type::fn_next
    fn_next_item_t next_item;

    // Can throw
    static void init_class_type();

    Iterator(const fn_next_item_t &next_item,
             Object *collection = nullptr, int_t custom_data = 0);
};
