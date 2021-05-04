#pragma once

// The base object class

#include "gc.hh"
#include "utils.hh"
#include <iterator>
#include <vector>
#include <unordered_map>

struct Type;
struct Object;

// Function that calls the visitor on all objects to traverse
typedef std::function<void(Object *child)> fn_visit_object_t;
typedef std::function<void(Object *obj, const fn_visit_object_t &visit)>
    fn_traverse_objects_t;

struct Object;

// Returns whether this object is a Plain Old Data type,
// that is, it must be copied on various operations
bool is_pod_object(Object *o);

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

    // Int type verified (or nullptr)
    // Compares references by default
    Object *cmp(Object *o);

    // Returns this if there is no custom handler
    Object *copy();

    Object *div(Object *o);

    Object *getattr(Object *name);

    Object *getitem(Object *key);

    // Returns the reference if there is no custom handler
    Object *hash();

    Object *in(Object *value);

    Object *iter();

    // Int type verified (or nullptr)
    Object *len();

    Object *mod(Object *o);

    Object *mul(Object *o);

    Object *neg();

    Object *next();

    Object *setattr(Object *name, Object *value);

    Object *setitem(Object *key, Object *value);

    // Str type verified (or nullptr)
    // Returns 'ObjectType()' by default
    Object *str();

    Object *sub(Object *o);
};

struct UserTypeType;

// Every type must have a unique instance of this class
struct Type : public Object {
    static Type *class_type;

    // Unique identifier
    unsigned int id;

    // Complete name
    str_t name;

    // This function is called with fn_call
    // Can be empty
    fn_ternary_t constructor;

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

    // / operator, returns the quotient
    fn_binary_t fn_div;

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

    // Returns the iterator from the first element of the collection
    fn_unary_t fn_iter;

    // Container length
    fn_unary_t fn_len;

    // % operator, returns the modulo
    fn_binary_t fn_mod;

    // * operator, returns the product
    fn_binary_t fn_mul;

    // - operator (unary), returns the negation, the opposite
    fn_unary_t fn_neg;

    // Next iter item (or enditer)
    fn_unary_t fn_next;

    // Set map attribute (not read only)
    fn_ternary_t fn_setattr;

    // Subscript setter, example : obj[42] = 2
    fn_ternary_t fn_setitem;

    // String representation
    // Returns a Str
    fn_unary_t fn_str;

    // % operator, returns the modulo
    fn_binary_t fn_sub;

    // A type is always global
    Type(const str_t &name, bool register_type = true);

    static void init_class_type();

    // Compares the unique identifier
    bool operator==(const Type &other) const;

protected:
    // Inits all slots
    // Not in init_class_type to be called from UserType (inherited)
    static void init_slots(Type *type);
};

// Object with DynamicType as type
// Methods and static attributes can be set
struct DynamicObject : public Object {
    std::unordered_map<str_t, Object*> attrs;

    using Object::Object;

    // Inits a dynamic object instance
    // Can throw
    static void init(DynamicObject *o);
};

// Type that has attributes within a map
struct DynamicType : public Type {
    static Type *class_type;

    std::unordered_map<str_t, Object*> attrs;

    static DynamicType *New(const str_t &name);

    static void init_class_type();

private:
    DynamicType(const str_t &name);
};
