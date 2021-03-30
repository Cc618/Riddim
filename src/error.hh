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
Error *AssertError(error_msg_t msg);
