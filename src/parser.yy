%skeleton "lalr1.cc"
%require "3.7.3"
%defines

%define api.token.raw
%define api.token.prefix {TOK_}
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

%code top {
    #include "driver.hh"

	static inline yy::parser::symbol_type yylex(Driver &driver) {
        return driver.next_token();
    }
}

// Parsing context
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

%token
    EOF 0       "<<EOF>>"
    ASSIGN      "="
    AND         "and"
    OR          "or"
    NOT         "not"
    EQ          "=="
    LE          "<="
    GE          ">="
    LESSER      "<"
    GREATER     ">"
    MINUS       "-"
    PLUS        "+"
    WILDCARD    "*"
    SLASH       "/"
    MOD         "%"
    LPAREN      "("
    RPAREN      ")"
    LBRACE      "{"
    RBRACE      "}"
    STOP        "<LF>"
    IF          "if"
    TRUE        "true"
    FALSE       "false"
    NULL        "null"
;

%token <str_t> ID "id"
%token <str_t> STR "string"
%token <int> INT "int"
%nterm <ast::Block*> block_content
%nterm <ast::Stmt*> stmt
%nterm <ast::ExpStmt*> expstmt
%nterm <ast::Set*> set
%nterm <ast::Exp*> exp
%nterm <ast::Const*> const
%nterm <ast::BinExp*> binexp
%nterm <ast::UnaExp*> unaexp
%nterm stop

// The lower it is declared, the sooner the token will be used
%left "=";
%left "or";
%left "and";
%left "not";
%left "==" "<=" ">=" "<" ">";
%left "+" "-";
%left "*" "/" "%";

%start module;

%%
module: block_content {
            driver.module = new AstModule(@$.begin.line);
            driver.module->content = $1;
        }
    | stop {
            // Empty
            driver.module = new AstModule(@$.begin.line);
        }
    ;

block_content: %empty { $$ = new Block(@$.begin.line); }
    | block_content stmt { $$ = $1; $$->stmts.push_back($2); }
    ;

stmt: expstmt { $$ = $1; }
    ;

expstmt: exp stop { $$ = new ExpStmt($1); }
    ;

exp: const { $$ = $1; }
    | binexp { $$ = $1; }
    | unaexp { $$ = $1; }
    | set { $$ = $1; }
    | ID { $$ = new Id(@1.begin.line, $1); }
    ;

set: ID "=" exp { $$ = new Set(@1.begin.line, $1, $3); }
    ;

// TODO
    // | exp "-" exp { $$ = new BinExp(@1.begin.line, $1, BinExp::Sub, $3); }
    // | exp "/" exp { $$ = new BinExp(@1.begin.line, $1, BinExp::Div, $3); }
    // | exp "%" exp { $$ = new BinExp(@1.begin.line, $1, BinExp::Mod, $3); }

binexp : exp "or" exp { $$ = new BinExp(@1.begin.line, $1, BinExp::Or, $3); }
    | exp "and" exp { $$ = new BinExp(@1.begin.line, $1, BinExp::And, $3); }
    | exp "==" exp { $$ = new BinExp(@1.begin.line, $1, BinExp::Equal, $3); }
    | exp "<=" exp { $$ = new BinExp(@1.begin.line, $1, BinExp::LesserEqual, $3); }
    | exp ">=" exp { $$ = new BinExp(@1.begin.line, $1, BinExp::GreaterEqual, $3); }
    | exp "<" exp { $$ = new BinExp(@1.begin.line, $1, BinExp::Lesser, $3); }
    | exp ">" exp { $$ = new BinExp(@1.begin.line, $1, BinExp::Greater, $3); }
    | exp "+" exp { $$ = new BinExp(@1.begin.line, $1, BinExp::Add, $3); }
    | exp "*" exp { $$ = new BinExp(@1.begin.line, $1, BinExp::Mul, $3); }
    ;

unaexp: "not" exp { $$ = new UnaExp(@1.begin.line, $2, UnaExp::Not); }
    ;

const: INT { $$ = new Const(@1.begin.line, $1); }
    | STR { $$ = new Const(@1.begin.line, $1); }
    | TRUE { $$ = new Const(@1.begin.line, Const::True); }
    | FALSE { $$ = new Const(@1.begin.line, Const::False); }
    | NULL { $$ = new Const(@1.begin.line, Const::Null); }
    ;

stop: STOP
    | stop STOP
    ;
%%

void yy::parser::error(const location_type &l, const std::string &m) {
    driver.error(l.begin.line, l.begin.column, l.end.line, l.end.column, m);
}
