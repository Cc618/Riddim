#pragma once

// Interprets code objects

#include "code.hh"
#include "frame.hh"
#include "module.hh"

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
    // Binds the frame of a lambda
    BindLambda,
    // - pop 2
    // - push TOS1 / TOS
    BinDiv,
    // Checks whether an item is within a collection
    // - pop 2
    // - push TOS1 in TOS
    BinIn,
    // Is type
    // - pop 2
    // - push TOS1->type == TOS
    BinIs,
    // - pop 2
    // - push TOS1 % TOS
    BinMod,
    // - pop 2
    // - push TOS1 * TOS
    BinMul,
    // - pop 2
    // - push TOS1 - TOS
    BinSub,
    // Compares two values
    // - pop 1
    // - push TOS1 <=> TOS
    BinThreeWay,
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
    // Tries to catch the error if it matches the specific type at the TOS
    // If caught, the exception is stored in a new variable named 'id'
    // * The id can be null if we don't want to store the error
    // - id_offset : Offset of the constant
    // - nomatch_offset : Offset where to jump if no such match
    // - pop 1
    // * The TOS can be null for a catch all (matches always)
    CatchError,
    // Terminates a catch block, this instruction is similar to Jmp except
    // that it clears the caught error
    CatchTerminate,
    // Duplicates the TOS
    Dup,
    // Duplicates TOS1 to the TOS
    DupTos1,
    // If it remains elements in the iterator, pushes the next element
    // Otherwise, jumps to the specified offset (end of loop)
    // - offset
    // - push @next(TOS) if != enditer, push nothing otherwise but Jmp at the
    // offset
    ForNext,
    // Jump at offset
    Jmp,
    // Jump if TOS is false
    // - pop 1
    JmpFalse,
    // TOS become TOS.name
    // - name : Offset of the name of the attribute
    // - pop 1
    // * Copies PODs
    LoadAttr,
    // Loads a const value on the TOS
    // - const_offset : The const object offset
    // - push 1
    // * Copies PODs
    LoadConst,
    // Loads indexed (or key mapped) value given as subscript
    // - pop 2
    // - push TOS1[TOS]
    // * Copies PODs
    LoadIndex,
    // Loads a module to the TOS
    // - modname : Module's name
    LoadModule,
    // Loads a variable (object associated to a symbol) on the TOS
    // - id_offset : Offset of the name of the symbol (constant)
    // - push 1
    // * Copies PODs
    LoadVar,
    // Makes a range object
    // - inclusive : Boolean, whether end is inclusive or exclusive
    // - pop 3 : start, end, step
    // - push the range object
    MakeRange,
    // Merges variables of the TOS (a Module) to the current frame
    // - pop 1
    MergeModule,
    // Registers a new type with its constructor on the TOS (may be null)
    // and pushes the new type to the TOS
    // - name : Name of the new type
    // - pop 1
    // - push 1
    NewType,
    // No operation, used usually as a placeholder during code generation
    Nop,
    // Packs 'count' variables of the stack in a Vec
    // - count : Number of variables to pack
    // - pop 'count'
    // - push a Vec of all variables to pack
    // * Element are stored in a LIFO order
    // * Copies PODs
    Pack,
    // Packs 'count' key-value pairs ('count' * 2 total variables) in a HashMap
    // The key is the first pushed value on the stack
    // - pop 'count' * 2
    // - push a HashMap of all key value pairs to pack
    // * Copies PODs
    PackMap,
    // Pops the TOS
    // - pop 1
    Pop,
    // Pops a try block of the current frame
    PopTryBlock,
    // Pushes a new Frame::TryBlock to the current frame
    // - catch_offset : See Frame::TryBlock::catch_offset
    PushTryBlock,
    // Dispatches the current caught error
    Rethrow,
    // Returns from the function (or exit the module)
    // The TOS is the return value, it must be set (can be null for "void"
    // functions)
    // * Copies PODs
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
    // - pop 1
    // - push @iter(TOS)
    UnaIter,
    // - pop 1
    // - push -TOS
    UnaNeg,
    // - pop 1
    // - push not TOS (must be bool)
    UnaNot,
    // - pop 1
    // - push TOS->type
    UnaTypeOf,
    // Unpacks 'count' variables to the TOS
    // - count : Required collection size
    // - push 'count' variables
    Unpack,
};
} // namespace OpCode

// Interprets the main module's code
// This prints the traceback in case of uncaught error
// Returns whether the program executed successfully
bool interpret_program(Module *main_module);

// Interprets the code
// - vars : The default variables of the frame
// Can throw
void interpret(Code *code, const str_t &id,
               const std::unordered_map<str_t, Object *> &vars = {},
               Module *module = nullptr, Frame *lambda_frame = nullptr);

// Interprets a fragment of code
// Compared to interpret, it doesn't create a new frame (it uses the top frame)
void interpret_fragment(Code *code, size_t &ip);
