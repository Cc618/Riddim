#pragma once

// Sets are just maps with a dummy value (null)

#include "map.hh"
#include "methods.hh"
#include "object.hh"

struct HashSet : public DynamicObject {
    static DynamicType *class_type;

    HashMap *data;

    static HashSet *New(HashMap *data = nullptr, bool create_data = true);

    // Can throw
    static void init_class_type();
    static void init_class_objects();

    // Inserts a new object
    DECL_METHOD(add);

    // Removes an object
    DECL_METHOD(pop);

protected:
    HashSet(HashMap *data);

private:
    static size_t class_hash;
};

struct TreeSet : public DynamicObject {
    static DynamicType *class_type;

    TreeMap *data;

    static TreeSet *New(TreeMap *data = nullptr, bool create_data = true);

    // Can throw
    static void init_class_type();
    static void init_class_objects();

    // Inserts a new object
    DECL_METHOD(add);

    // Removes an object
    DECL_METHOD(pop);

protected:
    TreeSet(TreeMap *data);

private:
    static size_t class_hash;
};
