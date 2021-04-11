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
%nterm <ast::Exp*> exp set boolean comparison binary unary primary
%nterm <ast::Target*> target target_id target_indexing target_attr
%nterm <ast::CallExp*> call call_args call_args_filled
%nterm <ast::Indexing*> indexing
%nterm <ast::Attr*> attr
// Atoms
%nterm <ast::Exp*> atom
%nterm <ast::Id*> id
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
%right "=";
%left "or";
%left "and";
%left "not";
%left "==" "<=" ">=" "<" ">" "is";
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
exp: set { $$ = $1; }
    ;

// Set and targets
// We can chain multiple sets
set: boolean { $$ = $1; }
    | target "=" set { $$ = new Set(@1.begin.line, $1, $3); }
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

boolean: comparison { $$ = $1; }
    | "not" comparison { $$ = new UnaExp(@1.begin.line, $2, UnaExp::Not); }
    | boolean "or" comparison { $$ = new BinExp(@1.begin.line, $1, BinExp::Or, $3); }
    | boolean "and" comparison { $$ = new BinExp(@1.begin.line, $1, BinExp::And, $3); }
    ;

comparison: binary { $$ = $1; }
    | comparison "==" binary { $$ = new BinExp(@1.begin.line, $1, BinExp::Equal, $3); }
    | comparison "<=" binary { $$ = new BinExp(@1.begin.line, $1, BinExp::LesserEqual, $3); }
    | comparison ">=" binary { $$ = new BinExp(@1.begin.line, $1, BinExp::GreaterEqual, $3); }
    | comparison "<" binary { $$ = new BinExp(@1.begin.line, $1, BinExp::Lesser, $3); }
    | comparison ">" binary { $$ = new BinExp(@1.begin.line, $1, BinExp::Greater, $3); }
    | comparison "is" binary { $$ = new BinExp(@1.begin.line, $1, BinExp::Is, $3); }
    | comparison "is" "not" binary { $$ = new BinExp(@1.begin.line, $1, BinExp::IsNot, $4); }
    ;

// TODO : Multiple comparisons
// comparison: binary
//     | binary comp_list
// comparison_list: "<" binary
//     | "is" binary
//     | "is" "not" binary

// Binary
// TODO
    // | exp "-" exp { $$ = new BinExp(@1.begin.line, $1, BinExp::Sub, $3); }
    // | exp "/" exp { $$ = new BinExp(@1.begin.line, $1, BinExp::Div, $3); }
    // | exp "%" exp { $$ = new BinExp(@1.begin.line, $1, BinExp::Mod, $3); }

binary : unary { $$ = $1; }
    | binary "+" unary { $$ = new BinExp(@1.begin.line, $1, BinExp::Add, $3); }
    | binary "*" unary { $$ = new BinExp(@1.begin.line, $1, BinExp::Mul, $3); }
    ;

// Unaries
unary: primary { $$ = $1; }
    ;

// Primaries
primary: atom { $$ = $1; }
    | lparen exp rparen { $$ = $2; }
    | call { $$ = $1; }
    | indexing { $$ = $1; }
    | attr { $$ = $1; }
    ;

attr: primary "." ID { $$ = new Attr(@1.begin.line, $1, $3); }
    ;

indexing: primary "[" exp "]" { $$ = new Indexing(@1.begin.line, $1, $3); }
    ;

// Call
call: primary lparen call_args rparen {
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

// --- Atoms ---
atom: const { $$ = $1; }
    | id { $$ = $1; }
    | vec { $$ = $1; }
    | map { $$ = $1; }
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

const: INT { $$ = new Const(@1.begin.line, $1); }
    | STR { $$ = new Const(@1.begin.line, $1); }
    // TODO : As global constants
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
