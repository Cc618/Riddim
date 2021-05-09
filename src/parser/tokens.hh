#pragma once

// Tokens recognized by the lexer (scanner.ll)

#include "driver.hh"
#include "utils.hh"
#include <string>

// Throws a lexer error
void lexer_error(const yy::parser::location_type &l, const std::string &msg);

yy::parser::symbol_type make_INT(const yy::parser::location_type &loc,
                                 const str_t &raw, const std::string &s,
                                 int base, bool negate = false);

yy::parser::symbol_type make_STR(const yy::parser::location_type &loc,
                                 const std::string &s);