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

    // Returns the iterator associated to the key
    // Can throw (returns data.end() on error)
    hmap_t::iterator find(Object *key);

    // Like getitem
    // Can throw
    Object *get(Object *key);

    // Like setitem
    // Can throw
    void set(Object *key, Object *value);

    static HashMap *New(const hmap_t &data = {});

    // Can throw
    static void init_class_type();
    static void init_class_objects();

protected:
    HashMap(const hmap_t &data = {});

private:
    static size_t class_hash;
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
    static void init_class_objects();

protected:
    AttrObject(HashMap *data = nullptr);

private:
    static size_t class_hash;
};