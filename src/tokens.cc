#include "tokens.hh"

using namespace std;
using namespace yy;

void lexer_error(const yy::parser::location_type &loc, const std::string &msg) {
    throw LexerError{loc.begin.line, loc.begin.column,
            loc.end.line, loc.end.column, msg};
}

parser::symbol_type make_INT(const str_t &s,
                             const parser::location_type &loc) {
    errno = 0;
    long n = strtol(s.c_str(), NULL, 10);

    if (errno == ERANGE)
        lexer_error(loc, str_t("Invalid integer: ") + s);

    return parser::make_INT(n, loc);
}

parser::symbol_type make_STR(const str_t &s,
                             const parser::location_type &loc) {
    errno = 0;

    // TODO : Parse escapes

    // Remove leading and trailing quotes
    return parser::make_STR(s.substr(1, s.size() - 2), loc);
}