#include "driver.hh"
#include "ast.hh"
#include "parser.hh"
#include <cstring>

using namespace std;

extern FILE *yyin;

Driver::Driver() {}

int Driver::parse(const string &f) {
    file = f;
    location.initialize(&file);

    scan_begin();
    // TODO : Try catch
    yy::parser parse(*this);
    int res = parse();

    scan_end();

    return res;
}

void Driver::scan_begin() {
    // TODO : Disable
    if (file.empty() || file == "-")
        yyin = stdin;
    else if (!(yyin = fopen(file.c_str(), "r"))) {
        cerr << "cannot open " << file << ": " << strerror(errno) << endl;
        exit(EXIT_FAILURE);
    }
}

void Driver::scan_end() { fclose(yyin); }
