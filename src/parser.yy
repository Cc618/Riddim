%skeleton "lalr1.cc"
%require "3.7.3"
%defines

%define api.token.raw

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires {
    #include <string>
    #include <vector>
    #include <iostream>
    #include "ast.hh"
    #include "utils.hh"

    class Driver;
}

// The parsing context.
%param { Driver& driver }

%locations

// Debugging
// %define parse.trace
// %define parse.error detailed
// %define parse.lac full
// %printer { yyo << $$; } <*>;

%code {
    #include "driver.hh"

    using namespace std;
    using namespace ast;
}

%define api.token.prefix {TOK_}
%token
    EQ          "="
    MINUS       "-"
    PLUS        "+"
    WILDCARD    "*"
    SLASH       "/"
    MOD         "%"
    LPAREN      "("
    RPAREN      ")"
    LBRACE      "{"
    RBRACE      "}"
    // TODO : Line feed
    STOP        ";"
    IF          "if"
    // TODO
    DEBUG       "@debug"
;

%token <str_t> ID "id"
%token <str_t> STR "string"
%token <int> INT "int"
%nterm <ast::Block*> block_content
%nterm <ast::Stmt*> stmt
%nterm <ast::Set*> set
%nterm <ast::Exp*> exp
%nterm <ast::Const*> const
%nterm <ast::BinExp*> binexp
%nterm stop

%left "+" "-";
%left "*" "/" "%";

%start module;

%%
module: block_content {
            driver.module = new Module(@$.begin.line);
            driver.module->content = $1; }
    ;

block_content: %empty { $$ = new Block(@$.begin.line); }
    | block_content stmt { $$ = $1; $$->stmts.push_back($2); }
    ;

stmt: set { $$ = $1; }
    ;

set: ID "=" exp stop { $$ = new Set(@1.begin.line, $1, $3); }
    ;

exp: const { $$ = $1; }
    | binexp { $$ = $1; }
    ;

binexp : exp "+" exp { $$ = new BinExp(@1.begin.line, $1, '+', $3); }
    |  exp "-" exp { $$ = new BinExp(@1.begin.line, $1, '-', $3); }
    |  exp "*" exp { $$ = new BinExp(@1.begin.line, $1, '*', $3); }
    |  exp "/" exp { $$ = new BinExp(@1.begin.line, $1, '/', $3); }
    |  exp "%" exp { $$ = new BinExp(@1.begin.line, $1, '%', $3); }
    ;

const: INT { $$ = new Const(@1.begin.line, $1); }
    | STR { $$ = new Const(@1.begin.line, $1); }
    ;

stop: STOP
    ;
%%

void yy::parser::error(const location_type &l, const std::string &m) {
    driver.error(l.begin.line, l.begin.column, l.end.line, l.end.column, m);
}
