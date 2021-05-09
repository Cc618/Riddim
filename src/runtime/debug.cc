#include "debug.hh"
#include <iostream>

using namespace std;

void debug_info(const str_t &msg) {
    cout << msg << endl;
}

void debug_warn(const str_t &msg) {
    cout << "[DEBUG:WARN] ";
    debug_info(msg);
}

void debug_err(const str_t &msg) {
    cerr << "[DEBUG:ERROR] " << msg << endl;
}