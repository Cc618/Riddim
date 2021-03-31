#pragma once

// Error object and functions

#include "object.hh"

// TODO : Use a String object / with a __dict__ ?
typedef str_t error_msg_t;

// Default error, can be instantiated but it is
// recommended to throw specific errors (AssertError...)
struct Error : public Object {
    static Type *class_type;

    error_msg_t msg;

    Error(error_msg_t msg);

    static void init_class_type();
};

// Some factories for commonly used errors
#define NEW_ERROR(TYPE)                                                        \
    extern Type *TYPE;                                                         \
    Error *New##TYPE(error_msg_t msg);

// !!! Don't forget to update the documentation when adding an error

// Zero division...
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
void dump_error();

// If !assertion, returns true and throws the AssertError with this message
// !!! Don't forget to return from the function
bool err_assert(bool assertion, const str_t &msg);

// !!! Don't forget to return from the function
void throw_str(Type *error_type, const str_t &msg);

// !!! Don't forget to return from the function
void throw_fmt(Type *error_type, const char *fmt, const char *args...);

// An error that can't be caught in Riddim
void internal_error(const str_t &msg);
