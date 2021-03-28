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

    class Driver;
}

// The parsing context.
%param { Driver& drv }

%locations

// Debugging
// %define parse.trace
// %define parse.error detailed
// %define parse.lac full
// %printer { yyo << $$; } <*>;

%code {
    #include "driver.hh"
    // TODO
    using namespace std;
}

%define api.token.prefix {TOK_}
%token
    EQ  "="
    // MINUS   "-"
    PLUS    "+"
    // STAR    "*"
    // SLASH   "/"
    LPAREN  "("
    RPAREN  ")"
    LBLOCK  "{"
    RBLOCK  "}"
    // TODO : Line feed
    STOP    ";"
    IF      "if"
    // TODO
    DEBUG   "@debug"
;

%token <std::string> ID "id"
%token <std::string> STR "string"
%token <int> INT "int"
%nterm <Block*> block_content
%nterm <Stmt*> stmt
%nterm <Set*> set
%nterm <Exp*> exp
%nterm <Const*> const
%nterm stop

// %left "+" "-";
// %left "*" "/" "%";

%start module;

%%
module: block_content { drv.module = new Module(); drv.module->content = $1; }
    ;

block_content: %empty { $$ = new Block(); }
    | block_content stmt { $$ = $1; $$->stmts.push_back($2); }
    ;

stmt: set { $$ = $1; }
    ;

set: ID "=" exp stop { $$ = new Set($1, $3); }
    ;

exp: const { $$ = $1; }
    ;

const: INT { $$ = new Const($1); }
    | STR { $$ = new Const($1); }
    ;

stop: STOP
    ;
%%

void yy::parser::error(const location_type &l, const std::string &m) {
    std::cerr << l << ": " << m << '\n';
}
