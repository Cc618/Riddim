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
    // Applies boolean operation
    // - op : The boolean operation
    // - pop 2
    // - push bool_binop(TOS1, TOS, op)
    BinBool,
    // Compares two values
    // - op : The boolean comparison
    // - pop 2
    // - push compare(TOS1, TOS, op)
    BinCmp,
    // Is type
    // - pop 2
    // - push TOS1->type == TOS
    BinIs,
    // - pop 2
    // - push TOS1 * TOS
    BinMul,
    // TODO : Self
    // Only positional args call
    // Calls TOS1 with TOS (a Vec) as positional args
    // - pop 2
    // - push result of the call
    Call,
    // Full call (contains at least kw args)
    // Calls TOS2 with TOS1 (an HashMap) and TOS (a Vec) as kw / pos args
    // - pop 3
    // - push result of the call
    CallKw,
    // Call without any args
    // Calls TOS
    // - pop 1
    // - push result of the call
    CallProc,
    // Duplicates the TOS
    Dup,
    // Jump at offset
    Jmp,
    // Jump if TOS is false
    // - pop 1
    JmpFalse,
    // TOS become TOS.name
    // - name : Offset of the name of the attribute
    // - pop 1
    LoadAttr,
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
    // No operation, used usually as a placeholder during code generation
    Nop,
    // Packs 'count' variables of the stack in a Vec
    // - count : Number of variables to pack
    // - pop 'count'
    // - push a Vec of all variables to pack
    // * Element are stored in a LIFO order
    Pack,
    // Packs 'count' key-value pairs ('count' * 2 total variables) in a HashMap
    // The key is the first pushed value on the stack
    // - pop 'count' * 2
    // - push a HashMap of all key value pairs to pack
    PackMap,
    // Pops the TOS
    // - pop 1
    Pop,
    // Returns from the function (or exit the module)
    Return,
    // TOS.name = TOS1
    // - name : Offset of the name of the attribute
    // - pop 1
    StoreAttr,
    // Stores indexed (or key mapped) value
    // TOS1[TOS] = TOS2
    // - pop 2
    StoreIndex,
    // Stores the TOS to a variable
    // - id_offset : Offset of the name of the symbol (constant)
    StoreVar,
    // TODO A : Update
    // - pop 3
    // - push TOS2(args=TOS1, kwargs=TOS)
    TerCall,
    // - pop 1
    // - push not TOS (must be bool)
    UnaNot,
    // - pop 1
    // - push TOS->type
    UnaTypeOf,
    // TODO : Remove
    // Prints the stack
    DebugStack,
};
} // namespace OpCode

// Interprets the code of a code frame
// Can throw
void interpret(Frame *frame);
