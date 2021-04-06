#pragma once

// Boolean data type

#include "object.hh"

struct Bool;

extern Bool *istrue;
extern Bool *isfalse;

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
