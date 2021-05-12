#pragma once

// Error object and functions
// Defines also an exception for the code generation (try catch used here)

#include "object.hh"
#include <exception>

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

// Unspecified error
Error *NewError(error_msg_t msg);

// Invalid argument count...
NEW_ERROR(ArgumentError);

// Zero division
NEW_ERROR(ArithmeticError);

// False assertion
NEW_ERROR(AssertError);

// Used by exit builtin
NEW_ERROR(ExitError);

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

// Max recursion depth reached
NEW_ERROR(RecursionError);

// Rethrow not within a try catch...
NEW_ERROR(RuntimeError);

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

// Invalid argument
static inline void THROW_ARGUMENT_ERROR(const str_t &FUNC, const str_t &ARG,
                                        const str_t &MSG) {
    throw_fmt(ArgumentError, "%s : Argument %s%s%s : %s",
        FUNC.c_str(), C_BLUE, ARG.c_str(), C_NORMAL, MSG.c_str());
}

#define THROW_MEMORY_ERROR throw_str(MemoryError, "Failed to allocate memory");

// Variable not found
// VAR is a string
#define THROW_NAME_ERROR(VAR)                                                  \
    throw_fmt(NameError, "Symbol '%s%s%s' not found", C_BLUE, (VAR).c_str(),   \
              C_NORMAL)

// Args are types
#define THROW_TYPE_ERROR(CURRENT, EXPECTED)                                    \
    throw_fmt(TypeError, "Got type %s%s%s but expected type %s%s%s", C_RED,    \
              (CURRENT)->name.c_str(), C_NORMAL, C_BLUE,                       \
              (EXPECTED)->name.c_str(), C_NORMAL)

// Invalid type with prefix
#define THROW_TYPE_ERROR_PREF(PREF, CURRENT, EXPECTED)                         \
    throw_fmt(TypeError, "%s : Got type %s%s%s but expected type %s%s%s",      \
              (PREF), C_RED, (CURRENT)->name.c_str(), C_NORMAL, C_BLUE,        \
              (EXPECTED)->name.c_str(), C_NORMAL)

// Attribute not found
// ATTR is a string and TYPE is a type
#define THROW_ATTR_ERROR(TYPE, ATTR)                                           \
    throw_fmt(NameError, "Attribute %s%s%s not found for type %s%s%s", C_BLUE, \
              (ATTR).c_str(), C_NORMAL, C_GREEN, (TYPE)->name.c_str(),         \
              C_NORMAL)

// Arithmetic error
#define THROW_ARITHMETIC_ERROR(OP, MSG)                                        \
    throw_fmt(ArithmeticError, "Operator %s%s%s : %s", C_BLUE, (OP), C_NORMAL, \
              (MSG))

// Throws a NameError that says no such builtin method
// TYPE is a type while METHOD is stringified
#define THROW_NOBUILTIN(TYPE, METHOD)                                          \
    throw_fmt(NameError, "Type %s%s%s has no %s@" #METHOD "%s method",         \
              C_GREEN, (TYPE)->name.c_str(), C_NORMAL, C_BLUE, C_NORMAL);

// No kwargs required
static inline void THROW_EXTRA_KWARGS(const str_t &FUNC, const str_t &EXTRA) {
    throw_fmt(ArgumentError, "%s%s%s : Extra kwarg (%s%s%s)", C_BLUE,
              FUNC.c_str(), C_NORMAL, C_BLUE, EXTRA.c_str(), C_NORMAL);
}

// LEN and IDX are ints
#define THROW_OUT_OF_BOUNDS(LEN, IDX)                                          \
    throw_fmt(IndexError,                                                      \
              "Index %s%d%s out of bounds for a collection of size %s%d%s",    \
              C_BLUE, (IDX), C_NORMAL, C_GREEN, (LEN), C_NORMAL);

#define THROW_STACK_TOOSMALL(MINLEN)                                           \
    throw_fmt(InternalError,                                                   \
              "Object stack of length %s%d%s too small (%s%d%s items needed)", \
              C_BLUE, (Program::instance->obj_stack.size()), C_NORMAL, C_BLUE, \
              (MINLEN), C_NORMAL);

class CodeGenException : public std::exception {
public:
    CodeGenException(const std::string &msg, const std::string &filename,
                     int lineno);

    virtual const char *what() const noexcept override;

private:
    str_t rawmsg;
};
