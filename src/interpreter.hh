#pragma once

// Interprets code objects

#include "frame.hh"

// enum class but we can use the using keyword to bring this scope in source
// code
namespace OpCode {
enum OpCode : opcode_t {
    // - pop 2
    // - push TOS1 + TOS
    BinAdd,
    // Loads a const value on the TOS
    // - const_offset : The const object offset
    // - push 1
    LoadConst,
    // Loads indexed (or key mapped) value given as subscript
    // - pop 2
    // - push TOS1[TOS]
    LoadIndex,
    // Loads a variable (object associated to a symbol) on the TOS
    // - id_offset : Offset of the name of the symbol (constant)
    // - push 1
    LoadVar,
    // Pops the TOS
    // - pop 1
    Pop,
    // Returns from the function (or exit the module)
    Return,
    // Stores indexed (or key mapped) value
    // TOS1[TOS] = TOS2
    // - pop 2
    StoreIndex,
    // Stores the TOS to a variable
    // - id_offset : Offset of the name of the symbol (constant)
    // - pop 1
    StoreVar,
    // TODO : Remove
    // Prints the stack
    DebugStack,
    // // Replaces TOS1, TOS2 by TOS1 + TOS2
    // // TOS = TOS2 + TOS1
    // Add,
    // // Pop TOS, if false then jumps to location
    // // - loc, size_t : Location
    // JmpFalse,
};
} // namespace OpCode

// Interprets the code of a code frame
// Can throw
void interpret(Frame *frame);
