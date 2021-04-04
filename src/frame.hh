#pragma once

// Code frame, contains variables within a scope (stack frame)
// and gathers also the code to interpret

#include "map.hh"
#include "vec.hh"
#include <vector>

// Code fragment
typedef std::vector<size_t> code_t;

struct Frame : public Object {
    static Type *class_type;

    Frame *previous;
    HashMap *vars;

    // Instruction pointer (counter)
    size_t ip;
    code_t code;

    // Constants, to be copied
    Vec *consts;

    // File info
    size_t start_lineno;
    str_t filename;

    // Ordered array where &[off, delta] = line_deltas[i] indicates that the
    // instruction at offset off starts delta lines after the previous
    // instruction
    std::vector<std::pair<size_t, size_t>> line_deltas;

    // It is like the getitem function
    // Fetch the first variable named "name"
    // This is the main method to resolve variable / function names at runtime
    // Returns nullptr if not found with a name error (throws)
    // TODO lambda : Return frame where the variable belong to too (to save it)
    Object *fetch(Object *name);

    // Returns the line number of the instruction at offset 'offset' within code
    size_t lineof(size_t offset);

    // Pushes a new constant to consts and returns its index
    size_t add_const(Object *cst);

    // Spawns (copies) the constant at index i
    Object *spawn_const(size_t i);

    // Can throw
    static Frame *New(Frame *previous = nullptr);

    // Can throw
    static void init_class_type();

private:
    Frame(Frame *previous);
};
