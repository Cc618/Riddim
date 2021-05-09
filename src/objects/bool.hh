#pragma once

// Boolean data type

#include "object.hh"
#include "interpreter.hh"

struct Bool;

extern Bool *istrue;
extern Bool *isfalse;

enum class CmpOp : opcode_t {
    Lesser,
    Greater,
    LesserEqual,
    GreaterEqual,
    Equal,
    NotEqual,
};

// Compares a with b
// Uses a->cmp
// Can throw
Bool *compare(Object *a, Object *b, CmpOp op);

enum class BoolBinOp : opcode_t {
    And,
    Or,
};

// Applies a op b
// * a and b are Bools
// Can throw
Bool *bool_binop(Object *a, Object *b, BoolBinOp op);

// Negates / inverse the boolean
// * o is Bool
// Can throw
Bool *bool_not(Object *o);

struct Bool : public Object {
    static Type *class_type;

    bool data;

    // Can throw
    static void init_class_type();

    // Can throw
    static void init_class_objects();

protected:
    Bool(bool data);
};
