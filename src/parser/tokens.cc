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

parser::symbol_type make_STR(const parser::location_type &loc, const str_t &s, bool is_doc_str) {
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

    if (is_doc_str)
        return parser::make_DOCSTR(result, loc);
    else
        return parser::make_STR(result, loc);
}

yy::parser::symbol_type make_RAWSTR(const yy::parser::location_type &loc,
                                    std::string s, bool is_doc_str) {
    static const string BLANK = "\t\n\r ";

    if (!s.empty()) {
        auto first = s.front() == '\n' != string::npos
                         ? s.find_first_not_of("\n")
                         : string::npos;

        if (first != string::npos && first > 0)
            s.erase(s.begin(), s.begin() + first);

        if (!s.empty()) {
            auto last = BLANK.find(s.back()) != string::npos
                            ? s.find_last_not_of(BLANK) + 1
                            : string::npos;

            if (last != string::npos && last < s.size())
                s.erase(s.begin() + last, s.end());
        }
    }

    // Handle indentation
    int min_indent = s.size();
    vector<int> indents;
    for (int i = 0; i < s.size(); ++i) {
        if (i == 0 || s[i] == '\n') {
            int current_indent = 0;

            if (i != 0)
                ++i;

            // Save the indent index
            indents.push_back(i);

            // Count indentation size
            for (; i < s.size() && (s[i] == ' ' || s[i] == '\t');
                 ++i, ++current_indent)
                ;

            min_indent = min(min_indent, current_indent);
        }
    }

    // Remove extra indentation
    if (min_indent != 0) {
        for (int i = indents.size() - 1; i >= 0; --i) {
            s.erase(indents[i], min_indent);
        }
    }

    // Remove leading and trailing quotes
    if (is_doc_str)
        return parser::make_DOCSTR(s, loc);
    else
        return parser::make_STR(s, loc);
}
