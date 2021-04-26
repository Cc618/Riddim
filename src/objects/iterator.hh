#pragma once

// Basic iterator type
// Note that we don't have to use this class to be iterable
// This class is just a convenient way to create an iterator
// * enditer is defined in builtins.hh

#include "object.hh"
#include <functional>

struct Iterator;

typedef std::function<Object *(Iterator *it)> fn_iterator_next_item_t;
typedef std::function<void(Iterator *it)> fn_iterator_delete_item_t;

struct Iterator : public Object {
    static Type *class_type;

    Object *collection;

    void *custom_data;

    // See Type::fn_next
    fn_iterator_next_item_t next_item;

    fn_iterator_delete_item_t delete_custom_data;

    // Can throw
    static void init_class_type();

    Iterator(const fn_iterator_next_item_t &next_item, Object *collection = nullptr,
             void *custom_data = nullptr,
             const fn_iterator_delete_item_t &delete_custom_data =
                 fn_iterator_delete_item_t());

    ~Iterator();
};
