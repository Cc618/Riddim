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
    NotEqual
    // TODO : TypeEqual ?
};

// Compares a with b
// Uses a->cmp
// Can throw
Bool *compare(Object *a, Object *b, CmpOp op);

struct Bool : public Object {
    static Type *class_type;

    bool data;

    // TODO : fn_istrue

    // Can throw
    static void init_class_type();

    // Can throw
    static void init_class_objects();

protected:
    Bool(bool data);
};
