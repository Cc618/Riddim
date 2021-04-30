#include "utils.hh"
#include "int.hh"
#include "program.hh"
#include "builtins.hh"
#include <filesystem>
#include <fstream>
#include <iostream>

using namespace std;
namespace fs = std::filesystem;

str_t string_repr(const str_t &s) {
    // TODO : Escape chars
    return "'" + s + "'";
}

void parse_error(const str_t &filename, int begin_line, int begin_col,
                 int end_line, int end_col, const str_t &msg) {
    Program::instance->errout << C_BLUE << filename << C_NORMAL << ":" << C_RED << begin_line
         << C_NORMAL << ":" << C_RED << begin_col << C_NORMAL
         << ": Error : " << msg << endl;

    show_source_line(filename, begin_line);
}

void show_source_line(const str_t &filename, int line) {
    auto text = read_source_line(filename, line);
    if (!text.empty())
        Program::instance->errout << C_RED << line << ". " << C_NORMAL << text << endl;
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

std::vector<int_t> try_collect_int_iterator(Object *iterable, int_t low, int_t high) {
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
            THROW_TYPE_ERROR_PREF("Iterator Collection", i->type, Int::class_type);

            return {};
        }

        int_t data = reinterpret_cast<Int*>(i)->data;

        if (data >= low && data < high) {
            result.push_back(data);
        }
    }

    return result;
}
