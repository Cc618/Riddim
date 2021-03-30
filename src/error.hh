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
extern Type *AssertError;
Error *NewAssertError(error_msg_t msg);

// Throws an error and updates the error indicator
void throw_error(Object *error);

// Clears the current error
void clear_error();

// Checks whether the current error is of this type
bool is_error(Type *type);

// Checks whehter an error occured
bool on_error();

// If !assertion, returns true and throws the AssertError with this message
// !!! Don't forget to return from the function
bool err_assert(bool assertion, const str_t &msg);
