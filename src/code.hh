#pragma once

// Code element, gathers the code to interpret of a code object

#include "map.hh"
#include "vec.hh"
#include <vector>

typedef size_t opcode_t;
typedef std::vector<opcode_t> code_t;

struct Code : public Object {
    static Type *class_type;

    // Instruction pointer (counter)
    size_t ip;
    code_t code;

    // Constants, to be copied
    Vec *consts;

    // File info
    size_t start_lineno;
    str_t filename;

    // Ordered array where &[off, delta] = line_deltas[i] indicates that the
    // instruction at offset off starts delta lines after start_lineno
    // (not after the previous instruction)
    std::vector<std::pair<size_t, size_t>> line_deltas;

    // Returns the line number of the instruction at offset 'offset' within code
    size_t lineof(size_t offset);

    // Updates line_deltas
    void mark_line(size_t lineno);

    // Pushes a new constant to consts and returns its index
    size_t add_const(Object *cst);

    // Spawns (copies) the constant at index i
    Object *spawn_const(size_t i);

    // Can throw
    static Code *New();

    // Can throw
    static void init_class_type();

private:
    Code();
};
