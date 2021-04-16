#pragma once

// Code frame, contains variables within a scope (stack frame)

#include "map.hh"
#include "vec.hh"
#include <vector>

typedef size_t opcode_t;
typedef std::vector<opcode_t> code_t;

// Use setitem to set variables
struct Frame : public Object {
    struct TryBlock {
        // Instruction offset where the first catch lies
        size_t catch_offset;
    };

    static Type *class_type;

    // Instruction pointer (counter)
    size_t ip;
    // Function name, module name...
    str_t id;
    str_t filename;

    Frame *previous;
    HashMap *vars;

    // Ordered array where &[off, delta] = line_deltas[i] indicates that the
    // instruction at offset off starts delta lines after start_lineno
    // (not after the previous instruction)
    std::vector<std::pair<size_t, size_t>> line_deltas;

    // For try catches, stack describing where is each first catch block
    std::vector<TryBlock> tryblocks;

    // It is like the getitem function
    // Fetch the first variable named "name"
    // This is the main method to resolve variable / function names at runtime
    // Returns nullptr if not found with a name error (throws)
    // TODO lambda : Return frame where the variable belong to too (to save it)
    Object *fetch(Object *name);

    // Can throw
    static Frame *New(const str_t &id, const str_t &filename, Frame *previous = nullptr);

    // Can throw
    static void init_class_type();

private:
    Frame(const str_t &id, const str_t &filename, Frame *previous);
};
