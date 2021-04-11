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
    IS          "is"
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
    LCURLY      "{"
    RCURLY      "}"
    LBRACK      "["
    RBRACK      "]"
    DOT         "."
    COMMA       ","
    COLON       ":"
    STOP        "<LF>"
    IF          "if"
    ELIF        "elif"
    ELSE        "else"
    WHILE       "while"
    TRUE        "true"
    FALSE       "false"
    NULL        "null"
;

// Declarations
%nterm <ast::Block*> block
%nterm <ast::Block*> stmtlist
// Statements
%nterm <ast::Stmt*> stmt
%nterm <ast::WhileStmt*> whilestmt
%nterm <ast::IfStmt*> ifstmt ifstmt_elif
%nterm <ast::Block*> ifstmt_else
%nterm <ast::ExpStmt*> expstmt
// Expressions
%nterm <ast::Exp*> exp primary
%nterm <ast::BinExp*> binary
%nterm <ast::UnaExp*> unary
%nterm <ast::Set*> set
%nterm <ast::Target*> target
%nterm <ast::IdTarget*> target_id
%nterm <ast::IndexingTarget*> target_indexing
%nterm <ast::AttrTarget*> target_attr
// Atoms
%nterm <ast::CallExp*> call call_args call_args_filled
%nterm <ast::Id*> id
%nterm <ast::Indexing*> indexing
%nterm <ast::Attr*> attr
%nterm <ast::VecLiteral*> vec
%nterm <ast::MapLiteral*> map
%nterm <std::vector<std::pair<ast::Exp *, ast::Exp *>>> map_content map_content_filled
%nterm <std::vector<ast::Exp*>> vec_content vec_content_filled
%nterm <ast::Const*> const
// Tokens
%nterm stop lcurly rcurly lparen rparen lbrack rbrack
%token <str_t> ID "id"
%token <str_t> STR "string"
%token <int_t> INT "int"

// The lower it is declared, the sooner the token will be used
// TODO
%left ISNOT;
%left "=";
%left "or";
%left "and";
%left "not";
%left "==" "<=" ">=" "<" ">";
%left "is";
%left "+" "-";
%left "*" "/" "%";
%left ".";
%left "[";
%left "{";
%left "(";

%start module;

%%
// --- AST ---
module: stop stmtlist {
            driver.module = new AstModule(@$.begin.line);
            driver.module->content = $2;
        }
    ;

// --- Declarations ---
block: lcurly stmtlist rcurly { $$ = $2; }
    | lcurly stop stmtlist rcurly { $$ = $3; }
    ;

// --- Statements ---
stmtlist: %empty { $$ = new Block(@$.begin.line); }
    | stmtlist stmt { $$ = $1; $$->stmts.push_back($2); }
    ;

stmt: expstmt { $$ = $1; }
    | ifstmt { $$ = $1; }
    | whilestmt { $$ = $1; }
    ;

ifstmt: "if" exp block ifstmt_elif stop {
        $$ = new IfStmt($2, $3);
        $$->elsebody = new Block(@4.begin.line);
        $$->elsebody->stmts.push_back($4);
    }
    | "if" exp block ifstmt_else stop {
        $$ = new IfStmt($2, $3);
        $$->elsebody = $4;
    }
    ;

ifstmt_elif: "elif" exp block ifstmt_elif {
        $$ = new IfStmt($2, $3);
        $$->elsebody = new Block(@4.begin.line);
        $$->elsebody->stmts.push_back($4);
    }
    | "elif" exp block ifstmt_else {
        $$ = new IfStmt($2, $3);
        $$->elsebody = $4;
    }
    ;

ifstmt_else: %empty { $$ = nullptr; }
    | "else" block {
        $$ = $2;
    }
    ;

whilestmt: "while" exp block stop { $$ = new WhileStmt($2, $3); }
    ;

expstmt: exp stop { $$ = new ExpStmt($1); }
    ;

// --- Expressions ---
exp: primary { $$ = $1; }
    | binary { $$ = $1; }
    | unary { $$ = $1; }
    | set { $$ = $1; }
    ;

// TODO
// exp: primary
//     | "not" primary { cout << "not" << endl; }
//     | primary "is" primary
//     | primary "is" "not" primary { cout << "is not" << endl; }
//     ;


// Set and targets
set: target "=" exp { $$ = new Set(@1.begin.line, $1, $3); }
    ;

target: target_id { $$ = $1; }
    | target_indexing { $$ = $1; }
    | target_attr { $$ = $1; }
    ;

target_indexing: indexing { $$ = new IndexingTarget($1); }
    ;

target_attr: attr { $$ = new AttrTarget($1); }
    ;

target_id: ID { $$ = new IdTarget(@1.begin.line, $1); }
    ;

primary: lparen exp rparen {}
    | const { $$ = $1; }
    | id { $$ = $1; }
    | indexing { $$ = $1; }
    | attr { $$ = $1; }
    | vec { $$ = $1; }
    | map { $$ = $1; }
    | call { $$ = $1; }
    ;

// Binary
// TODO
    // | exp "-" exp { $$ = new BinExp(@1.begin.line, $1, BinExp::Sub, $3); }
    // | exp "/" exp { $$ = new BinExp(@1.begin.line, $1, BinExp::Div, $3); }
    // | exp "%" exp { $$ = new BinExp(@1.begin.line, $1, BinExp::Mod, $3); }

binary : exp "or" exp { $$ = new BinExp(@1.begin.line, $1, BinExp::Or, $3); }
    | exp "and" exp { $$ = new BinExp(@1.begin.line, $1, BinExp::And, $3); }
    | exp "==" exp { $$ = new BinExp(@1.begin.line, $1, BinExp::Equal, $3); }
    | exp "<=" exp { $$ = new BinExp(@1.begin.line, $1, BinExp::LesserEqual, $3); }
    | exp ">=" exp { $$ = new BinExp(@1.begin.line, $1, BinExp::GreaterEqual, $3); }
    | exp "<" exp { $$ = new BinExp(@1.begin.line, $1, BinExp::Lesser, $3); }
    | exp ">" exp { $$ = new BinExp(@1.begin.line, $1, BinExp::Greater, $3); }
    | exp "+" exp { $$ = new BinExp(@1.begin.line, $1, BinExp::Add, $3); }
    | exp "*" exp { $$ = new BinExp(@1.begin.line, $1, BinExp::Mul, $3); }
    | exp "is" exp { $$ = new BinExp(@1.begin.line, $1, BinExp::Is, $3); }
    | exp "is" "not" exp %prec ISNOT { $$ = new BinExp(@1.begin.line, $1, BinExp::IsNot, $4); }
    ;

unary: "not" exp { $$ = new UnaExp(@1.begin.line, $2, UnaExp::Not); }
    ;


// --- Atoms ---
// Call
call: exp lparen call_args rparen {
        $$ = $3;
        $$->fileline = @1.begin.line;
        $$->exp = $1;
    }
    ;

call_args: %empty { $$ = new CallExp(0); }
    | call_args_filled { $$ = $1; }
    | call_args_filled "," { $$ = $1; }
    ;

call_args_filled: ID ":" exp {
        $$ = new CallExp(0);
        $$->kwargs.push_back({ $1, $3 });
    }
    | exp {
        $$ = new CallExp(0);
        $$->args.push_back($1);
    }
    | call_args_filled "," exp { $$ = $1; $$->args.push_back($3); }
    | call_args_filled "," stop exp { $$ = $1; $$->args.push_back($4); }
    | call_args_filled "," ID ":" exp { $$ = $1; $$->kwargs.push_back({ $3, $5 }); }
    | call_args_filled "," stop ID ":" exp { $$ = $1; $$->kwargs.push_back({ $4, $6 }); }
    ;

// Maps
map: lcurly map_content rcurly { $$ = new MapLiteral(@1.begin.line, $2); }
    ;

map_content: %empty { $$ = {}; }
    | map_content_filled { $$ = $1; }
    | map_content_filled "," { $$ = $1; }
    ;

map_content_filled: exp ":" exp { $$ = {{ $1, $3 }}; }
    | map_content_filled "," exp ":" exp { $$ = $1; $$.push_back({ $3, $5 }); }
    | map_content_filled "," stop exp ":" exp { $$ = $1; $$.push_back({ $4, $6 }); }
    ;

// Vec
vec: lbrack vec_content rbrack { $$ = new VecLiteral(@1.begin.line, $2); }
    ;

vec_content: %empty { $$ = {}; }
    | vec_content_filled { $$ = $1; }
    | vec_content_filled "," { $$ = $1; }
    ;

vec_content_filled: exp { $$ = { $1 }; }
    | vec_content_filled "," exp { $$ = $1; $$.push_back($3); }
    | vec_content_filled "," stop exp { $$ = $1; $$.push_back($4); }
    ;

attr: exp "." ID { $$ = new Attr(@1.begin.line, $1, $3); }
    ;

indexing: exp "[" exp "]" { $$ = new Indexing(@1.begin.line, $1, $3); }
    ;

const: INT { $$ = new Const(@1.begin.line, $1); }
    | STR { $$ = new Const(@1.begin.line, $1); }
    | TRUE { $$ = new Const(@1.begin.line, Const::True); }
    | FALSE { $$ = new Const(@1.begin.line, Const::False); }
    | NULL { $$ = new Const(@1.begin.line, Const::Null); }
    ;

id: ID { $$ = new Id(@1.begin.line, $1); }
    ;

// --- Tokens ---
stop: STOP
    | stop STOP
    ;

lcurly: LCURLY
    ;

rcurly: RCURLY
    ;

lbrack: LBRACK
    ;

rbrack: RBRACK
    ;

lparen: LPAREN
    ;

rparen: RPAREN
    ;
%%

void yy::parser::error(const location_type &l, const std::string &m) {
    driver.error(l.begin.line, l.begin.column, l.end.line, l.end.column, m);
}
