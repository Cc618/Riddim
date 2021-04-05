#pragma once

// Interprets code objects

#include "frame.hh"

// enum class but we can use the using keyword to bring this scope in source code
namespace OpCode {
enum OpCode : opcode_t {
    // // Pop TOS
    // Pop,
    // // Loads a const value on the TOS
    // // - const_offset : The const object offset
    LoadConst,
    // Loads a variable (object associated to a symbol) on the TOS
    // - id_offset : Offset of the name of the symbol (constant)
    // LoadVar,
    // // Stores the TOS to a variable
    // // - id_offset : Offset of the name of the symbol (constant)
    // StoreVar,
    // // Replaces TOS1, TOS2 by TOS1 + TOS2
    // // TOS = TOS2 + TOS1
    // Add,
    // // Pop TOS, if false then jumps to location
    // // - loc, size_t : Location
    // JmpFalse,
    // Returns from the function (or exit the module)
    // - value_offset : Offset of the return value (can be null but must be set)
    Return,
    // TODO : Remove
    // Prints the stack
    DebugStack,
};
} // namespace OpCode

// Interprets the code of a code frame
// Can throw
void interpret(Frame *frame);
