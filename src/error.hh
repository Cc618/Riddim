#pragma once

// Error object and functions

#include "object.hh"

typedef str_t error_msg_t;

// Default error, can be instantiated but it is
// recommended to throw specific errors (AssertError...)
struct Error : public Object {
    static Type *class_type;

    error_msg_t msg;

    // Can throw
    static void init_class_type();

    Error(error_msg_t msg);
};

// Some factories for commonly used errors
#define NEW_ERROR(TYPE)                                                        \
    extern Type *TYPE;                                                         \
    Error *New##TYPE(error_msg_t msg);

// !!! Don't forget to update the documentation when adding an error

// Invalid argument count...
NEW_ERROR(ArgumentError);

// Zero division
NEW_ERROR(ArithmeticError);

// False assertion
NEW_ERROR(AssertError);

// Import not found
NEW_ERROR(ImportError);

// Out of bounds / key not found on a collection
NEW_ERROR(IndexError);

// An error thrown only in the C++ API (ex: invalid format on throw error)
// This error can be thrown outside of the Riddim code execution
NEW_ERROR(InternalError);

// Invalid use of iterator
NEW_ERROR(IterError);

// Out of memory
NEW_ERROR(MemoryError);

// Local / attribute / global variable not found
NEW_ERROR(NameError);

// This method is not implemented yet
NEW_ERROR(NotImplementedError);

// Null reference
NEW_ERROR(NullError);

// Invalid type
NEW_ERROR(TypeError);

// TODO : File IO errors

#undef NEW_ERROR

// Throws an error and updates the error indicator
void throw_error(Object *error);

// Clears the current error
void clear_error();

// Checks whether the current error is of this type
bool is_error(Type *type);

// Checks whehter an error occured
bool on_error();

// Shows the current error
// This function can't throw other errors
void dump_error();

// If !assertion, returns true and throws the AssertError with this message
// !!! Don't forget to return from the function
bool err_assert(bool assertion, const str_t &msg);

// !!! Don't forget to return from the function
void throw_str(Type *error_type, const str_t &msg);

// Throw string and format it like in printf
// !!! Don't forget to return from the function
void throw_fmt(Type *error_type, const char *fmt, ...);

// An error that can't be caught in Riddim
void internal_error(const str_t &msg);

// TODO : Error colors
// Invalid argument
static inline void THROW_ARGUMENT_ERROR(const str_t &FUNC, const str_t &ARG,
                                 const str_t &MSG) {
    throw_fmt(ArgumentError, "%s : Argument %s :  %s", FUNC.c_str(),
              ARG.c_str(), MSG.c_str());
}

#define THROW_MEMORY_ERROR throw_str(MemoryError, "Failed to allocate memory");

// Variable not found
// VAR is a string and TYPE is a type
#define THROW_NAME_ERROR(VAR)                                                  \
    throw_fmt(NameError, "Symbol %s not found", (VAR).c_str())

// TODO : Check whether we throw TypeError in another way
// Args are types
#define THROW_TYPE_ERROR(CURRENT, EXPECTED)                                    \
    throw_fmt(TypeError, "Got type %s but expected type %s",                   \
              (CURRENT)->name.c_str(), (EXPECTED)->name.c_str())

// Invalid type with prefix
#define THROW_TYPE_ERROR_PREF(PREF, CURRENT, EXPECTED)                         \
    throw_fmt(TypeError, "%s : Got type %s but expected type %s", (PREF),      \
              (CURRENT)->name.c_str(), (EXPECTED)->name.c_str())

// Attribute not found
// ATTR is a string and TYPE is a type
#define THROW_ATTR_ERROR(TYPE, ATTR)                                           \
    throw_fmt(NameError, "Attribute %s not found for type %s", (ATTR).c_str(), \
              (TYPE)->name.c_str())

// Arithmetic error
#define THROW_ARITHMETIC_ERROR(OP, MSG)                                        \
    throw_fmt(ArithmeticError, "Operator %s : %s", (OP), (MSG))

// Throws a NameError that says no such builtin method
#define THROW_NOBUILTIN(METHOD)                                                \
    throw_fmt(NameError, "Type %s has no @" #METHOD " method",                 \
              type->name.c_str());

// No kwargs required
static inline void THROW_EXTRA_KWARGS(const str_t &FUNC, const str_t &EXTRA) {
    throw_fmt(ArgumentError, "%s : Extra keyword named argument (%s)",
              FUNC.c_str(), EXTRA.c_str());
}

// LEN and IDX are ints
#define THROW_OUT_OF_BOUNDS(LEN, IDX)                                          \
    throw_fmt(IndexError,                                                      \
              "Index %d out of bounds for a collection of size %d", (LEN),     \
              (IDX));

#define THROW_STACK_TOOSMALL(MINLEN)                                           \
    throw_fmt(InternalError,                                                   \
              "Object stack of length %d too small (%d items needed)",         \
              (Program::instance->obj_stack.size()), (MINLEN));
