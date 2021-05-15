#pragma once

#include "code.hh"
#include "object.hh"
#include "str.hh"
#include "frame.hh"

struct Module;

// Merges merged's frame into base
// Special variables like !doc / mod... are not merged
void merge_frames(Frame *base, Frame *merged);

struct Module : public Object {
    static Type *class_type;

    str_t filepath;
    Str *name;
    Code *code;
    Frame *frame;

    // Whether it is not fully executed, used to avoid
    // executing 2 times a module already loaded
    bool loaded = false;

    // To avoid infinite import loop
    bool being_loaded = false;

    static Module *New(const str_t &name, const str_t &filepath);

    // Can throw
    static void init_class_type();

private:
    Module(Str *name, Code *code, Frame *frame, const str_t &filepath);
};
