#pragma once

#include "frame.hh"
#include "object.hh"
#include "str.hh"

struct Module : public Object {
    static Type *class_type;

    // TODO : Str object ?
    str_t filepath;
    Str *name;
    Frame *frame;

    static Module *New(const str_t &name, const str_t &filepath);

    // Can throw
    static void init_class_type();

private:
    Module(Str *name, Frame *frame, const str_t &filepath);
};
