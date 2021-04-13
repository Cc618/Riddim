#pragma once

// Functor objects

#include "code.hh"
#include "map.hh"
#include "object.hh"
#include "utils.hh"

struct Frame;

// Function object
struct AbstractFunction : public Object {
    // null by default
    Object *self;

    AbstractFunction(Type *type, Object *self = nullptr);
};

// Built in function
// Wraps a C++ function
struct Function : public AbstractFunction {
    static Type *class_type;

    // (Object *self, Vec *args, HashMap *kwargs) -> Object*
    fn_ternary_t data;

    // Can throw
    static void init_class_type();

    Function(const fn_ternary_t &data, Object *self = nullptr);
};

// A function containing a code frame
// This kind of function is the one generated via function declarations
struct CodeFunction : public AbstractFunction {
    static Type *class_type;

    str_t name;
    Code *code;
    // Positional args with possibly default values (can be nullptr)
    std::vector<std::pair<str_t, Code*>> args;
    int n_required_args;

    static CodeFunction *New(Code *code, const str_t &name,
                             Object *self = nullptr);

    // Can throw
    static void init_class_type();

protected:
    CodeFunction(Code *code, const str_t &name,
                 Object *self);
};
