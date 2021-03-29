#pragma once

// The base object class

#include "gc.hh"
#include "utils.hh"
#include <iterator>
#include <vector>

// TODO
void testObjects();

struct Type;

// A global object can't be destroyed by the garbage collector
// where local objects can (global objects can't be instanced
// from Riddim itself)
struct Object {
    GcData gc_data;

    Type *type;

    // Can't be deleted by the GC
    static Object *Global();

    // Normal object
    static Object *Local();

    virtual ~Object();

protected:
    Object();
};

// Function that calls the visitor on all objects to traverse
typedef std::function<void(Object *obj,
                           std::function<void(Object *child)> visit)>
    fn_traverse_objects_t;

// Every type must have a unique instance of this class
struct Type : public Object {
    // Unique identifier
    unsigned int id;

    // Complete name
    str_t name;

    // The tp_traverse function
    fn_traverse_objects_t traverse_objects;

    // TODO :
    // fn_unary_t fn_new;
    // fn_unary_t fn_del;
    // fn_unary_t fn_hash;
    // fn_binary_t fn_cmp;
    // fn_binary_t fn_add;
    // fn_binary_t fn_sub;
    // fn_binary_t fn_mul;
    // fn_binary_t fn_div;
    // fn_binary_t fn_mod;
    // fn_unary_t fn_call;

    // TODO : Always global ?
    // A type is always global
    Type(const str_t &name);
};
