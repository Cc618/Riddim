#include "tokens.hh"

using namespace std;
using namespace yy;

parser::symbol_type make_INT(const str_t &s,
                             const parser::location_type &loc) {
    errno = 0;
    long n = strtol(s.c_str(), NULL, 10);

    // TODO : Err
    if (errno == ERANGE)
        throw parser::syntax_error(loc, "Invalid integer " + s);

    return parser::make_INT(n, loc);
}

parser::symbol_type make_STR(const str_t &s,
                             const parser::location_type &loc) {
    errno = 0;

    // TODO : Parse escapes

    // Remove leading and trailing quotes
    return parser::make_STR(s.substr(1, s.size() - 2), loc);
}