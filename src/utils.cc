#include "utils.hh"
#include <iostream>
#include <fstream>

using namespace std;

str_t string_repr(const str_t &s) {
    // TODO : Escape chars
    return "'" + s + "'";
}

// TODO : Colors
void parse_error(const str_t &filename, int begin_line, int begin_col, int end_line, int end_col, const str_t &msg) {
    cerr << filename << ":" << begin_line << ":" << begin_col << ": Error : " << msg << endl;

    ifstream f(filename);
    if (f.is_open()) {
        str_t line;
        for (int i = 1; i <= begin_line && getline(f, line); ++i);

        f.close();

        cerr << begin_line << ". " << line << endl;
    }
}
