#pragma once

// Tokens recognized by the lexer (scanner.ll)

#include "driver.hh"
#include <string>

// Throws a lexer error
void lexer_error(const yy::parser::location_type &l, const std::string &msg);

yy::parser::symbol_type make_INT(const std::string &s,
                                 const yy::parser::location_type &loc);

yy::parser::symbol_type make_STR(const std::string &s,
                                 const yy::parser::location_type &loc);