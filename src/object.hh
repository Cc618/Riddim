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

    Object *add(Object *o);

    Object *call(Object *args, Object *kwargs);

    // Compares references by default
    Object *cmp(Object *o);

    // Returns this if there is no custom handler
    Object *copy();

    Object *getattr(Object *name);

    Object *getitem(Object *key);

    // Returns the reference if there is no custom handler
    Object *hash();

    Object *in(Object *value);

    Object *mul(Object *o);

    Object *setattr(Object *name, Object *value);

    Object *setitem(Object *key, Object *value);

    // Returns ObjectType() by default
    Object *str();
};

// Every type must have a unique instance of this class
struct Type : public Object {
    static Type *class_type;

    // Unique identifier
    unsigned int id;

    // Complete name
    str_t name;

    // Builtin methods : Use wrappers within Object
    // The tp_traverse function
    fn_traverse_objects_t fn_traverse_objects;

    // + operator, returns the sum
    fn_binary_t fn_add;

    // Functor call
    // If the function doesn't return, null is returned
    // (not nullptr)
    // The kwargs can be null
    fn_ternary_t fn_call;

    // <=> operator, compares to another object
    // Returns < 0 if lesser or different, > 0 if greater, 0 if equal
    // * Should return not equal with null
    // * Should return lesser if not equal (if there is no specific order)
    fn_binary_t fn_cmp;

    // Returns a deep copy of this object
    fn_unary_t fn_copy;

    // Get map attribute (not read only)
    fn_binary_t fn_getattr;

    // Subscript getter, example : obj[42]
    fn_binary_t fn_getitem;

    // Used for hash tables etc...
    // Returns an Int
    fn_unary_t fn_hash;

    // Whether an object is within a collection
    // Returns a Bool
    fn_binary_t fn_in;

    // * operator, returns the product
    fn_binary_t fn_mul;

    // Set map attribute (not read only)
    fn_ternary_t fn_setattr;

    // Subscript setter, example : obj[42] = 2
    fn_ternary_t fn_setitem;

    // String representation
    // Returns a Str
    fn_unary_t fn_str;

    // TODO : + doc
    // fn_unary_t fn_init;
    // fn_unary_t fn_del;
    // fn_binary_t fn_cmp;
    // fn_binary_t fn_add;
    // fn_binary_t fn_sub;
    // fn_binary_t fn_mul;
    // fn_binary_t fn_div;
    // fn_binary_t fn_mod;
    // fn_unary_t fn_call;

    // A type is always global
    Type(const str_t &name);

    static void init_class_type();

    // Compares the unique identifier
    bool operator==(const Type &other) const;
};
