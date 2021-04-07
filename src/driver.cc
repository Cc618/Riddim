#include "driver.hh"
#include "ast.hh"
#include "parser.hh"
#include <cstring>

using namespace std;

extern FILE *yyin;

Driver::Driver() {}

int Driver::parse(const string &f) {
    // TODO : Convert to absolute path
    file = f;
    location.initialize(&file);

    try {
        scan_begin();

        yy::parser parse(*this);
        int res = parse();

        // Parsing OK
        if (res == 0)
            module->filename = f;

        scan_end();

        return res;
    } catch (LexerError e) {
        parse_error(file, e.begin_line, e.begin_column, e.end_line,
                    e.end_column, e.msg);
    }

    return -1;
}

void Driver::scan_begin() {
    if (!(yyin = fopen(file.c_str(), "r"))) {
        cerr << "Error : Cannot open " << file << " : " << strerror(errno)
             << endl;
        exit(EXIT_FAILURE);
    }
}

void Driver::scan_end() { fclose(yyin); }

void Driver::error(int begin_line, int begin_col, int end_line, int end_col,
                   const str_t &msg) {
    parse_error(file, begin_line, begin_col, end_line, end_col, msg);
}

#define parse yy::parser
parse::symbol_type Driver::next_token() {

    static bool ended = false;
    static parse::location_type eof_pos;

    if (ended) {
        ended = false;

        return parse::symbol_type(parse::token::TOK_EOF, eof_pos);
    }

    // Fetch next token
    auto tok = raw_yylex(*this);

    // Check if end of file
    if (tok.kind() == parse::symbol_kind::S_YYEOF) {
        ended = true;
        eof_pos = tok.location;

        // Add stop at the end of file
        return parse::symbol_type(parse::token::TOK_STOP, eof_pos);
    }

    return tok;
}
#undef parse
