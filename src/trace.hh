#pragma once

// Error stack trace
// Program holds a stack of traces, the TOS is the oldest trace

#include "object.hh"
#include "code.hh"

struct Trace : public Object {
    static Type *class_type;

    size_t ip;
    Code *code;
    Trace *prev = nullptr;

    static Trace *New(size_t ip, Code *code);

    // Can throw
    static void init_class_type();

private:
    Trace(size_t ip, Code *code);
};