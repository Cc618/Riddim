#pragma once

// Parser driver class

#include "ast.hh"
#include "parser.hh"
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

public:
    // Current file being parsed
    std::string file;
    yy::location location;

    Module *module;
};
