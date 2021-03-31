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
    // This attribute describes the type of this object
    // It is present also on other classes such as Type
    static Type *class_type;

    GcData gc_data;

    Type *type;

    Object(Type *type);

    // This static function inits the static attribute class_type
    // It is present on other objects such as Type
    // This function should be called in init_types
    // Can throw for most objects
    static void init_class_type();

    virtual ~Object();

    // Built-in methods, wrappers of Type::fn_*
    // See Type::fn_* for more details
    // Can throw and return nullptr (except for traverse_objects)
    void traverse_objects(const fn_visit_object_t &visit);
    Object *index(Object *args);
    Object *hash();
    Object *str();
};

// Every type must have a unique instance of this class
struct Type : public Object {
    static Type *class_type;

    // Unique identifier
    unsigned int id;

    // Complete name
    str_t name;

    // The tp_traverse function
    fn_traverse_objects_t fn_traverse_objects;

    // Subscript, example : obj[42]
    fn_binary_t fn_index;

    // Used for hash tables etc...
    // Returns an Int
    fn_unary_t fn_hash;

    // String representation
    // Returns a Str
    fn_unary_t fn_str;

    // TODO : + doc
    // fn_unary_t fn_init;
    // fn_unary_t fn_del;
    // fn_binary_t fn_in;
    // fn_binary_t fn_cmp;
    // fn_binary_t fn_add;
    // fn_binary_t fn_sub;
    // fn_binary_t fn_mul;
    // fn_binary_t fn_div;
    // fn_binary_t fn_mod;
    // fn_unary_t fn_call;
    // Map *attrs; // Can't be written if starts by @

    // A type is always global
    Type(const str_t &name);

    static void init_class_type();

    // Compares the unique identifier
    bool operator==(const Type &other) const;
};
