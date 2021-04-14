#pragma once

// Error stack trace
// Program holds a stack of traces, the TOS is the oldest trace

#include "code.hh"
#include "object.hh"

struct Trace : public Object {
    static Type *class_type;

    // Function name / module name...
    str_t id;
    size_t ip;
    // Contains filename
    Code *code;
    Trace *prev = nullptr;

    // Display the trace back for this frame only
    str_t display();

    // Dumps the whole stack trace (also previous traces)
    // on stderr
    void dump(int level = 1);

    static Trace *New(size_t ip, Code *code, const str_t &id);

    // Can throw
    static void init_class_type();

private:
    Trace(size_t ip, Code *code, const str_t &id);
};