#pragma once

// Map objects, they are equivalent to dicts in Python

#include "object.hh"
#include <map>

// Internal HashMap (stores hash, key / value pair)
typedef std::unordered_map<size_t, std::pair<Object *, Object *>> hmap_t;

// An unordered map that provides a mapping in O(1) average time
// (up to O(N))
struct HashMap : public Object {
    static Type *class_type;

    hmap_t data;

    HashMap();

    // TODO : As type lambda ?
    // Can throw
    Object *fn_index(Object *key);

    // TODO : As slot
    // Can throw
    void fn_setindex(Object *key, Object *value);

    // Returns the iterator associated to the key
    // Can throw (returns data.end() on error)
    hmap_t::iterator find(Object *key);


    // Can throw
    static void init_class_type();
};
