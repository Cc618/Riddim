#pragma once

#include "code.hh"
#include "object.hh"
#include "str.hh"
#include "frame.hh"

struct Module : public Object {
    static Type *class_type;

    str_t filepath;
    Str *name;
    Code *code;
    Frame *frame;

    static Module *New(const str_t &name, const str_t &filepath);

    // Can throw
    static void init_class_type();

private:
    Module(Str *name, Code *code, const str_t &filepath);
};
