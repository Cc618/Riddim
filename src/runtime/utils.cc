#include "utils.hh"
#include "builtins.hh"
#include "int.hh"
#include "program.hh"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>

using namespace std;
namespace fs = std::filesystem;

// Do not use without parentheses
#define REG_INT_DEC R"(([0-9][0-9_']*))"
#define REG_FLOAT_DEC REG_INT_DEC "\\." REG_INT_DEC
#define REG_FLOAT_EXP_PREF(PREF) PREF "e-?" REG_INT_DEC
#define REG_FLOAT_EXP                                                          \
    "(" REG_FLOAT_EXP_PREF(REG_INT_DEC) ")|(" REG_FLOAT_EXP_PREF(              \
        REG_FLOAT_DEC) ")"

// Same as in scanner.ll
static const auto int_dec = regex(REG_INT_DEC);
static const auto int_hex = regex(R"(0[xX][0-9a-fA-F][0-9a-fA-F_']*)");
static const auto int_bin = regex(R"(0[bB][01][01_']*)");
static const auto float_dec = regex(REG_FLOAT_DEC);
static const auto float_exp = regex(REG_FLOAT_EXP);

str_t string_repr(const str_t &s) {
    // TODO : Escape chars
    return "'" + s + "'";
}

optional<int_t> str_to_int(str_t s, int base) {
    if (s.empty()) {
        return {};
    }

    bool negate = false;

    if (base == -1) {
        // It is possible to have a negative number
        if (s[0] == '-') {
            negate = true;
            s = s.substr(1);
        }

        if (regex_match(s, int_dec)) {
            base = 10;
        } else if (regex_match(s, int_hex)) {
            base = 16;
            s = s.substr(2);
        } else if (regex_match(s, int_bin)) {
            base = 2;
            s = s.substr(2);
        } else {
            return {};
        }
    }

    // Valid
    s.erase(remove_if(s.begin(), s.end(),
                      [](char c) { return c == '\'' || c == '_'; }),
            s.end());

    try {
        long long n = stoll(s.c_str(), NULL, base);

        return negate ? -n : n;
    } catch (...) {
        return {};
    }
}

optional<float_t> str_to_float(str_t s, bool check_regex) {
    if (s.empty()) {
        return {};
    }

    bool negate = false;

    if (check_regex) {
        // Can be negative
        if (s[0] == '-') {
            s = s.substr(1);
            negate = true;
        }

        // Can be int too
        if (!regex_match(s, float_dec) && !regex_match(s, float_exp) &&
                !regex_match(s, int_dec)) {
            return {};
        }
    }

    s.erase(remove_if(s.begin(), s.end(),
                      [](char c) { return c == '\'' || c == '_'; }),
            s.end());

    // TODO C : Check precision
    try {
        size_t idx;
        float_t value = stod(s, &idx);

        if (idx != s.size()) {
            return {};
        }

        return negate ? -value : value;
    } catch (...) {
        return {};
    }
}

bool is_special_var(const str_t &s, bool allow_mod) {
    if (s.empty() || s[0] == '!' || s[0] == '@') {
        return true;
    }

    if (!allow_mod && s == "mod") {
        return true;
    }

    return false;
}

void parse_error(const str_t &filename, int begin_line, int begin_col,
                 int end_line, int end_col, const str_t &msg) {
    Program::instance->errout << C_BLUE << filename << C_NORMAL << ":" << C_RED
                              << begin_line << C_NORMAL << ":" << C_RED
                              << begin_col << C_NORMAL << ": Error : " << msg
                              << endl;

    show_source_line(filename, begin_line);
}

void show_source_line(const str_t &filename, int line) {
    auto text = read_source_line(filename, line);
    if (!text.empty())
        Program::instance->errout << C_RED << line << ". " << C_NORMAL << text
                                  << endl;
}

str_t read_source_line(const str_t &filename, int line) {
    ifstream f(filename);
    if (f.is_open()) {
        str_t sline;
        for (int i = 1; i <= line && getline(f, sline); ++i)
            ;

        f.close();

        return sline;
    }

    return "";
}

str_t module_name(const str_t &path) {
    try {
        fs::path filepath(path);

        auto extension = filepath.extension();

        if (extension != ".rid")
            return "";

        return filepath.stem();
    } catch (...) {
        return "";
    }
}

str_t abs_path(const str_t &path) {
    try {
        return fs::canonical(path).string();
    } catch (...) {
        return "";
    }
}

str_t dir_path(const str_t &path) {
    try {
        return fs::canonical(path).parent_path().string();
    } catch (...) {
        return "";
    }
}

bool is_file(const str_t &path) { return fs::is_regular_file(path); }

std::vector<int_t> try_collect_int_iterator(Object *iterable, int_t low,
                                            int_t high) {
    vector<int_t> result;

    auto iter = iterable->iter();

    if (!iter) {
        return {};
    }

    // Get all values within the range
    Object *i = nullptr;
    while (true) {
        i = iter->next();

        // Error
        if (i == nullptr) {
            return {};
        }

        // End of iterator
        if (i == enditer) {
            break;
        }

        if (i->type != Int::class_type) {
            THROW_TYPE_ERROR_PREF("Iterator Collection", i->type,
                                  Int::class_type);

            return {};
        }

        int_t data = reinterpret_cast<Int *>(i)->data;

        if (data >= low && data < high) {
            result.push_back(data);
        }
    }

    return result;
}
