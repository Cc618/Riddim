#pragma once

// Tokens recognized by the lexer (scanner.ll)

#include "driver.hh"
#include "utils.hh"
#include <string>

// Throws a lexer error
void lexer_error(const yy::parser::location_type &l, const std::string &msg);

yy::parser::symbol_type make_INT(const str_t &raw, const std::string &s,
                                 const yy::parser::location_type &loc, int base);

yy::parser::symbol_type make_STR(const std::string &s,
                                 const yy::parser::location_type &loc);