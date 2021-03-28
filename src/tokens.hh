#pragma once

// Tokens recognized by the lexer (scanner.ll)

#include "driver.hh"
#include <string>

yy::parser::symbol_type make_INT(const std::string &s,
                                 const yy::parser::location_type &loc);

yy::parser::symbol_type make_STR(const std::string &s,
                                 const yy::parser::location_type &loc);