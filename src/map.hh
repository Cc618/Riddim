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
    Object *getitem(Object *key);

    // TODO : As slot
    // Can throw
    void setitem(Object *key, Object *value);

    // Can throw
    static void init_class_type();

private:
    // Returns the iterator associated to the key
    // Can throw (returns data.end() on error)
    hmap_t::iterator find(Object *key);
};
