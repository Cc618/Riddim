#pragma once

// Map objects, they are equivalent to dicts in Python
// Defines also AttrObject, an object that allows custom attributes (using an
// HashMap)

#include "object.hh"
#include <map>

// Internal HashMap (stores hash, key / value pair)
typedef std::unordered_map<size_t, std::pair<Object *, Object *>> hmap_t;

// An unordered map that provides a mapping in O(1) average time
// (up to O(N))
struct HashMap : public Object {
    static Type *class_type;
    // An hashmap without objects, do not modify
    static HashMap *empty;

    hmap_t data;

    HashMap(const hmap_t &data = {});

    // Returns the iterator associated to the key
    // Can throw (returns data.end() on error)
    hmap_t::iterator find(Object *key);

    // Can throw
    Object *get(Object *key);

    // Can throw
    void set(Object *key, Object *value);

    // Can throw
    static void init_class_type();
    static void init_class_objects();
};

// An object that can contains attributes
// These attributes are set /
struct AttrObject : public Object {
    static Type *class_type;

    HashMap *data;

    // Constructor
    static AttrObject *New();

    // Can throw
    static void init_class_type();

protected:
    AttrObject(HashMap *data = nullptr);
};
