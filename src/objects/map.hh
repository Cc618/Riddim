#pragma once

// Map objects, they are equivalent to dicts in Python
// Defines also AttrObject, an object that allows custom attributes (using an
// HashMap)

#include "object.hh"
#include "methods.hh"
#include <map>

// Internal HashMap (stores hash, key / value pair)
typedef std::unordered_map<size_t, std::pair<Object *, Object *>> hmap_t;

// An unordered map that provides a mapping in O(1) average time
// (up to O(N))
struct HashMap : public DynamicObject {
    static DynamicType *class_type;

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

    // Removes a mapping by key
    DECL_METHOD(pop);

protected:
    HashMap(const hmap_t &data = {});

private:
    static size_t class_hash;
};

struct TreeMapCompare {
    bool operator()(Object *x, Object *y) const;
};

// Internal TreeMap
typedef std::map<Object *, Object *, TreeMapCompare> tmap_t;

// A TreeMap, stores key / value pairs in an ordered way (BST)
struct TreeMap : public DynamicObject {
    static DynamicType *class_type;

    // An treemap without objects, do not modify
    static TreeMap *empty;

    tmap_t data;

    static TreeMap *New(const tmap_t &data = {});

    // Can throw
    static void init_class_type();
    static void init_class_objects();

protected:
    TreeMap(const tmap_t &data = {});

private:
    static size_t class_hash;
};

// An object that can contain attributes
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
