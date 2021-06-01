#pragma once

// Sets are just maps with a dummy value (null)

#include "object.hh"
#include "methods.hh"
#include "map.hh"

struct HashSet : public DynamicObject {
    static DynamicType *class_type;

    HashMap *data;

    static HashSet *New();

    // Can throw
    static void init_class_type();
    static void init_class_objects();

    // Inserts a new object
    DECL_METHOD(add);

    // Removes an object
    DECL_METHOD(pop);

protected:
    HashSet();

private:
    static size_t class_hash;
};

// TODO A
// // A TreeSet, stores key / value pairs in an ordered way (BST)
// struct TreeSet : public DynamicObject {
//     static DynamicType *class_type;

//     // An treemap without objects, do not modify
//     static TreeSet *empty;

//     tmap_t data;

//     static TreeSet *New(const tmap_t &data = {});

//     // Can throw
//     static void init_class_type();
//     static void init_class_objects();

// protected:
//     TreeSet(const tmap_t &data = {});

// private:
//     static size_t class_hash;
// };

// // An object that can contain attributes
// struct AttrObject : public Object {
//     static Type *class_type;

//     HashSet *data;

//     // Constructor
//     static AttrObject *New();

//     // Can throw
//     static void init_class_type();
//     static void init_class_objects();

// protected:
//     AttrObject(HashSet *data = nullptr);

// private:
//     static size_t class_hash;
// };