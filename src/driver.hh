#pragma once

// Parser driver class

#include "ast.hh"
#include "parser.hh"
#include "utils.hh"
#include <string>

// Declare yylex
#define YY_DECL yy::parser::symbol_type yylex(Driver &drv)
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

public:
    // Current file being parsed
    std::string file;
    yy::location location;

    ast::Module *module;
};
