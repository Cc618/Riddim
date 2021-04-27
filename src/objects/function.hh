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
struct Builtin : public AbstractFunction {
    static Type *class_type;

    // (Object *self, Vec *args, HashMap *kwargs) -> Object*
    fn_ternary_t data;
    str_t name;

    // Can throw
    static void init_class_type();

    Builtin(const fn_ternary_t &data, const str_t &name, Object *self = nullptr);
};

// A function containing code to interpret
// This kind of function is the one generated via function declarations
struct Function : public AbstractFunction {
    static Type *class_type;

    str_t name;
    Code *code;
    // Positional args with possibly default values (can be nullptr)
    std::vector<std::pair<str_t, Code*>> args;
    int n_required_args;
    std::unordered_map<str_t, Object *> lambda_vars;

    static Function *New(Code *code, const str_t &name,
                             Object *self = nullptr);

    // Can throw
    static void init_class_type();

protected:
    Function(Code *code, const str_t &name,
                 Object *self);
};
