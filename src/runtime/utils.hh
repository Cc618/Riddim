#pragma once

// Some utility functions and definitions

#include <functional>
#include <string>
#include <optional>
#include <vector>

#define C_RED "\x1b[1;31m"
#define C_GREEN "\x1b[1;32m"
#define C_BLUE "\x1b[1;34m"
#define C_NORMAL "\x1b[1;0m"

struct Vec;
struct HashMap;
struct Object;

typedef std::string str_t;
typedef long long int_t;
typedef double float_t;
typedef unsigned int line_t;

// To be able to use the :: operator
#define args_t Vec
#define kwargs_t HashMap

typedef std::function<Object *(Object *)> fn_unary_t;
typedef std::function<Object *(Object *, Object *)> fn_binary_t;
typedef std::function<Object *(Object *, Object *, Object *)> fn_ternary_t;

// Returns a string representation of this string
// (Escapes chars...)
str_t string_repr(const str_t &s);

// Whether it is a special variabel
bool is_special_var(const str_t &s, bool allow_mod = true);

// Thrown from the lexer
struct LexerError {
    int begin_line;
    int begin_column;
    int end_line;
    int end_column;
    str_t msg;
};

void parse_error(const str_t &filename, int begin_line, int begin_col,
                 int end_line, int end_col, const str_t &msg);

// Displays on stderr the content of this file at this line
// Nothing is shown if the file can't be open
void show_source_line(const str_t &filename, int line);

// Returns the content of the line in filename
str_t read_source_line(const str_t &filename, int line);

// Returns the name of the module
// If the extension is not .rid or the path is invalid, returns an empty string
str_t module_name(const str_t &path);

// Absolute path
str_t abs_path(const str_t &path);

// The parent directory
str_t dir_path(const str_t &path);

// Exists and is readible
bool is_file(const str_t &path);

// Tries to collect a range like iterator with values from low (included)
// to high (excluded)
// Can throw
std::vector<int_t> try_collect_int_iterator(Object *iterable, int_t low, int_t high);
