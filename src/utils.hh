#pragma once

// Some utility functions and definitions

#include <string>
#include <functional>

struct Vec;
struct HashMap;
struct Object;

typedef std::string str_t;
typedef long long int_t;
typedef unsigned int line_t;

typedef Vec args_t;
typedef HashMap kwargs_t;

typedef std::function<Object *(Object *)> fn_unary_t;
typedef std::function<Object *(Object *, Object *)> fn_binary_t;
typedef std::function<Object *(Object *, Object *, Object *)> fn_ternary_t;

// Returns a string representation of this string
// (Escapes chars...)
str_t string_repr(const str_t &s);

// Thrown from the lexer
struct LexerError {
    int begin_line;
    int begin_column;
    int end_line;
    int end_column;
    str_t msg;
};

void parse_error(const str_t &filename, int begin_line, int begin_col, int end_line, int end_col, const str_t &msg);
