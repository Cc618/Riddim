#include "tokens.hh"
#include <climits>
#include <algorithm>

using namespace std;
using namespace yy;

void lexer_error(const yy::parser::location_type &loc, const std::string &msg) {
    throw LexerError{loc.begin.line, loc.begin.column,
            loc.end.line, loc.end.column, msg};
}

parser::symbol_type make_INT(const str_t &s,
                             const parser::location_type &loc) {
    string new_s = s;
    remove_if(new_s.begin(), new_s.end(), [](char c) { return c == '\'' || c == '_'; });
    long n = strtol(new_s.c_str(), NULL, 0);

    if (errno == ERANGE) {
        lexer_error(loc, str_t("Invalid integer: ") + s);
    }

    return parser::make_INT(n, loc);
}

parser::symbol_type make_STR(const str_t &s,
                             const parser::location_type &loc) {
    errno = 0;

    str_t result;

    // Maximum s.size() - 2 characters
    result.reserve(s.size() - 2);

    for (size_t i = 1; i < s.size() - 1; ++i) {
        // Parse escapes
        if (s[i] == '\\') {
            ++i;

            switch (s[i])
            {
            case '\'':
                result += '\'';
                break;

            case '"':
                result += '"';
                break;

            case '\\':
                result += '\\';
                break;

            case 'n':
                result += '\n';
                break;

            case 't':
                result += '\t';
                break;

            default:
                // Error (just output the raw escape)
                result += '\\';
                result += s[i];
            }
        } else result += s[i];
    }

    // Remove leading and trailing quotes
    return parser::make_STR(result, loc);
}
