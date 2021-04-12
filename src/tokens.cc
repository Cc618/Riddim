#include "tokens.hh"
#include <algorithm>
#include <climits>

using namespace std;
using namespace yy;

void lexer_error(const yy::parser::location_type &loc, const std::string &msg) {
    parse_error(Driver::instance->file, loc.begin.line, loc.begin.column,
                loc.end.line, loc.end.column, msg);

    Driver::instance->error_occured = true;
}

parser::symbol_type make_INT(const parser::location_type &loc, const str_t &raw,
                             const str_t &s, int base, bool negate) {
    string new_s = s;
    new_s.erase(remove_if(new_s.begin(), new_s.end(),
                          [](char c) { return c == '\'' || c == '_'; }),
                new_s.end());

    try {
        long long n = stoll(new_s.c_str(), NULL, base);

        return parser::make_INT(negate ? -n : n, loc);
    } catch (...) {
        // Out of range
        lexer_error(loc, str_t("Invalid integer: ") + raw);

        return parser::make_INT(0, loc);
    }
}

parser::symbol_type make_STR(const parser::location_type &loc, const str_t &s) {
    errno = 0;

    str_t result;

    // Maximum s.size() - 2 characters
    result.reserve(s.size() - 2);

    for (size_t i = 1; i < s.size() - 1; ++i) {
        // Parse escapes
        if (s[i] == '\\') {
            ++i;

            switch (s[i]) {
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
        } else
            result += s[i];
    }

    // Remove leading and trailing quotes
    return parser::make_STR(result, loc);
}
