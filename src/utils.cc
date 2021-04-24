#include "utils.hh"
#include <fstream>
#include <iostream>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

str_t string_repr(const str_t &s) {
    // TODO : Escape chars
    return "'" + s + "'";
}

// TODO : Colors
void parse_error(const str_t &filename, int begin_line, int begin_col,
                 int end_line, int end_col, const str_t &msg) {
    cerr << filename << ":" << begin_line << ":" << begin_col
         << ": Error : " << msg << endl;

    show_source_line(filename, begin_line);
}

void show_source_line(const str_t &filename, int line) {
    auto text = read_source_line(filename, line);
    if (!text.empty())
        cerr << C_RED << line << ". " << C_NORMAL << text << endl;
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
    fs::path filepath(path);

    auto extension = filepath.extension();

    if (extension != ".rid") return "";

    return filepath.stem();
}

str_t abs_path(const str_t &path) {
    return fs::canonical(path).string();
}
