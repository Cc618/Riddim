#pragma once

// Some utility functions and definitions

#include <string>

typedef std::string str_t;
typedef long long int_t;
typedef unsigned int line_t;

// Returns a string representation of this string
// (Escapes chars...)
str_t string_repr(const str_t &s);
