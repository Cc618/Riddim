#pragma once

// Parser driver class

#include "ast.hh"
#include "parser.hh"
#include "utils.hh"
#include <string>

// Declare yylex
#define YY_DECL yy::parser::symbol_type raw_yylex(Driver &drv)
YY_DECL;

class Driver {
public:
    Driver();

public:
    // 0 if ok
    // The result is this->module
    int parse(const std::string &f);

    void scan_begin();

    void scan_end();

    void error(int begin_line, int begin_col, int end_line, int end_col,
               const str_t &msg);

    // yylex but wrapped to add a new line at the end of the stream
    yy::parser::symbol_type next_token();

public:
    // Current file being parsed
    std::string file;
    yy::location location;

    ast::Module *module;
};
