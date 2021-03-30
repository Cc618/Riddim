#pragma once

// The base object class

#include "gc.hh"
#include "utils.hh"
#include <iterator>
#include <vector>

// TODO
void testObjects();

struct Type;
struct Object;

// Function that calls the visitor on all objects to traverse
typedef std::function<void(Object *child)> fn_visit_object_t;
typedef std::function<void(Object *obj, const fn_visit_object_t &visit)>
    fn_traverse_objects_t;

struct Object {
    // This attribute designs the type of this object
    // It is present also on other classes such as Type
    static Type *class_type;

    GcData gc_data;

    Type *type;

    virtual ~Object();

    // Wrappers of Type::fn_*
    void traverse_objects(const fn_visit_object_t &visit);

    Object(Type *type);
};

// Every type must have a unique instance of this class
struct Type : public Object {
    static Type *class_type;

    // Unique identifier
    unsigned int id;

    // Complete name
    str_t name;

    // The tp_traverse function
    // Can be empty
    fn_traverse_objects_t fn_traverse_objects;

    // TODO :
    // fn_unary_t fn_new;
    // fn_unary_t fn_del;
    // fn_unary_t fn_hash;
    // fn_unary_t fn_str;
    // fn_binary_t fn_cmp;
    // fn_binary_t fn_add;
    // fn_binary_t fn_sub;
    // fn_binary_t fn_mul;
    // fn_binary_t fn_div;
    // fn_binary_t fn_mod;
    // fn_unary_t fn_call;
    // Map *attrs; // Can't be written if starts by @

    // TODO : Overload operator == (check ids)

    // A type is always global
    Type(const str_t &name);
};
