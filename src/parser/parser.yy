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
    RADD        "+="
    RSUB        "-="
    RMUL        "*="
    RDIV        "/="
    RMOD        "%="
    AND         "and"
    OR          "or"
    NOT         "not"
    IS          "is"
    IN          "in"
    EQ          "=="
    NEQ         "!="
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
    CASCADE     ".."
    RANGE       "->"
    RANGEINC    "->="
    DOT         "."
    COMMA       ","
    COLON       ":"
    STOP        "<LF>"
    IF          "if"
    ELIF        "elif"
    ELSE        "else"
    USE         "use"
    WHILE       "while"
    FOR         "for"
    TRY         "try"
    CATCH       "catch"
    AS          "as"
    RETHROW     "rethrow"
    BREAK       "break"
    CONTINUE    "continue"
    RETURN      "return"
    PRINT       "print"
    THROW       "throw"
    FN          "fn"
;

// Declarations
%nterm <ast::FnDecl*> fndecl
%nterm <ast::FnDecl::Args*> fndecl_all_args fndecl_args fndecl_kwargs
%nterm <ast::Target*> fndecl_target
%nterm <ast::AttrTarget*> fndecl_attrtarget
%nterm <ast::IdTarget*> fndecl_idtarget
%nterm <ast::Block*> block
%nterm <ast::Block*> stmtlist

// Statements
%nterm <ast::Stmt*> stmt
%nterm <ast::UseStmt*> usestmt usestmt_start
%nterm <ast::WhileStmt*> whilestmt
%nterm <ast::ForStmt*> forstmt
%nterm <ast::IfStmt*> ifstmt ifstmt_elif
%nterm <ast::TryStmt*> trystmt trystmt_start
%nterm <ast::TryStmt::CatchClause> trystmt_catch trystmt_catchall
%nterm <ast::Block*> ifstmt_else
%nterm <ast::ReturnStmt*> returnstmt
%nterm <ast::LoopControlStmt*> loopcontrolstmt
%nterm <ast::RethrowStmt*> rethrowstmt
%nterm <ast::ExpStmt*> expstmt
%nterm <ast::Stmt*> macrostmt
%nterm <str_t> macro_keyword macro_keyword_single macro_keyword_varargs

// Expressions
%nterm <ast::Exp*> exp set /* TODO B : cascade */ boolean comparison binary unary primary
%nterm <ast::Target*> target target_id target_indexing target_attr target_multi
%nterm <ast::CallExp*> call call_args call_args_filled
%nterm <ast::Indexing*> indexing
%nterm <ast::Attr*> attr
%nterm <std::vector<ast::Target*>> target_multi_content

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
%nterm option_stop option_comma option_comma_stop
%token <str_t> ID
%token <str_t> STR
%token <int_t> INT

// The lower it is declared, the sooner the token will be used
%right "=" "+=" "-=" "*=" "/=" "%=";
%left "..";
%left "or";
%left "and";
%left "not";
%left "==" "<=" ">=" "<" ">" "is" "in";
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
// Function declaration
fndecl: "fn" fndecl_target lparen fndecl_all_args rparen block stop {
        $$ = new FnDecl(@1.begin.line, $2, $4, $6);
    }
    ;

fndecl_target: fndecl_idtarget { $$ = $1; }
    | fndecl_attrtarget { $$ = $1; }
    ;

fndecl_attrtarget: ID "." ID {
        $$ = new AttrTarget(
            new Attr(@1.begin.line,
                new Id(@1.begin.line, $1), $3));
    }
    | fndecl_attrtarget "." ID {
        $$ = $1;
        auto newattr = new Attr(@1.begin.line, $1->attr, $3);
        $1->attr = newattr;
    }
    ;

fndecl_idtarget: ID { $$ = new IdTarget(@1.begin.line, new Id(@1.begin.line, $1)); }
    ;

// kwargs are arguments with default values
fndecl_all_args: option_stop { $$ = new FnDecl::Args(); }
    | option_stop fndecl_args option_comma_stop  { $$ = $2; $$->n_required = $$->args.size(); }
    | option_stop fndecl_kwargs option_comma_stop { $$ = $2; }
    | option_stop fndecl_args "," option_stop fndecl_kwargs option_stop {
        $$ = $2;
        $$->n_required = $$->args.size();

        // Merge args in order (default values after)
        $$->args.insert($$->args.end(), $5->args.begin(), $5->args.end());
        $5->args.clear();

        delete $5;
    }
    ;

fndecl_args: ID { $$ = new FnDecl::Args(); $$->args.push_back({ $1, nullptr }); }
    | fndecl_args "," option_stop ID { $$ = $1; $$->args.push_back({ $4, nullptr }); }
    ;

fndecl_kwargs:
    ID ":" exp { $$ = new FnDecl::Args(); $$->args.push_back({ $1, $3 }); }
    | fndecl_kwargs "," option_stop ID ":" exp { $$ = $1; $$->args.push_back({ $4, $6 }); }
    ;

block: lcurly option_stop stmtlist rcurly { $$ = $3; }
    ;

// --- Statements ---
stmtlist: %empty { $$ = new Block(@$.begin.line); }
    | stmtlist stmt { $$ = $1; $$->stmts.push_back($2); }
    ;

stmt: expstmt { $$ = $1; }
    | ifstmt { $$ = $1; }
    | trystmt { $$ = $1; }
    | whilestmt { $$ = $1; }
    | usestmt { $$ = $1; }
    | forstmt { $$ = $1; }
    | returnstmt { $$ = $1; }
    | loopcontrolstmt { $$ = $1; }
    | rethrowstmt { $$ = $1; }
    | macrostmt { $$ = $1; }
    | fndecl { $$ = $1; }
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

trystmt: trystmt_start trystmt_catch stop {
        $$ = $1;
        $$->catchbodies.push_back($2);
    }
    | trystmt_start trystmt_catchall stop {
        $$ = $1;
        $$->catchbodies.push_back($2);
    }
    ;

trystmt_start: "try" block {
        $$ = new TryStmt($2);
    }
    | trystmt_start trystmt_catch {
        $$ = $1;
        $$->catchbodies.push_back($2);
    }
    ;

trystmt_catchall: "catch" ID block {
        $$ = TryStmt::CatchClause{nullptr, $2, $3};
    }
    ;

trystmt_catch: "catch" exp "as" ID block {
        $$ = TryStmt::CatchClause{$2, $4, $5};
    }
    ;

returnstmt: "return" exp stop { $$ = new ReturnStmt(@1.begin.line, $2); }
    | "return" stop { $$ = new ReturnStmt(@1.begin.line); }
    ;

loopcontrolstmt: "break" stop { $$ = new LoopControlStmt(@1.begin.line, true); }
    | "continue" stop { $$ = new LoopControlStmt(@1.begin.line, false); }
    ;

rethrowstmt: "rethrow" stop { $$ = new RethrowStmt(@1.begin.line); }
    ;

usestmt: usestmt_start stop { $$ = $1; }
    | usestmt_start "." "*" stop { $$ = $1; $$->asname = ""; }
    | usestmt_start "as" ID stop { $$ = $1; $$->asname = $3; }
    ;

usestmt_start: "use" ID { $$ = new UseStmt(@1.begin.line, $2, $2); }
    | usestmt_start "." ID { $$ = $1; $$->modname += "." + $3; $$->asname = $3; }
    ;

whilestmt: "while" exp block stop { $$ = new WhileStmt(@1.begin.line, $2, $3); }
    ;

forstmt: "for" target_id "in" exp block stop {
        $$ = new ForStmt(@1.begin.line, static_cast<IdTarget*>($2), $4, $5);
    }
    ;

expstmt: exp stop { $$ = new ExpStmt($1); }
    ;

macrostmt: macro_keyword_varargs vec_content_filled stop {
        auto call = new CallExp(@1.begin.line);
        call->args = $2;
        call->fileline = @1.begin.line;
        call->exp = new Id(@1.begin.line, $1);
        call->ismacro = true;

        $$ = new ExpStmt(call);
    }
    | macro_keyword_single exp stop {
        auto call = new CallExp(@1.begin.line);
        call->args = { $2 };
        call->fileline = @1.begin.line;
        call->exp = new Id(@1.begin.line, $1);
        call->ismacro = true;

        $$ = new ExpStmt(call);
    }
    ;

macro_keyword: macro_keyword_varargs { $$ = $1; }
    | macro_keyword_single { $$ = $1; }
    | "return" { $$ = "return"; }
    ;

macro_keyword_varargs: "print" { $$ = "print"; }
    ;

// Macro keyword that accepts only one argument
macro_keyword_single: "throw" { $$ = "throw"; }
    ;

// --- Expressions ---
exp: set { $$ = $1; }
    ;

// Set and targets
// We can chain multiple sets
set: boolean { $$ = $1; }
    | target "=" set { $$ = new Set(@1.begin.line, $1, $3); }
    | target "+=" set { $$ = new RelativeSet(@1.begin.line, $1, BinExp::Add, $3); }
    | target "-=" set { $$ = new RelativeSet(@1.begin.line, $1, BinExp::Sub, $3); }
    | target "*=" set { $$ = new RelativeSet(@1.begin.line, $1, BinExp::Mul, $3); }
    | target "/=" set { $$ = new RelativeSet(@1.begin.line, $1, BinExp::Div, $3); }
    | target "%=" set { $$ = new RelativeSet(@1.begin.line, $1, BinExp::Mod, $3); }
    ;

target: target_id { $$ = $1; }
    | target_indexing { $$ = $1; }
    | target_attr { $$ = $1; }
    | target_multi { $$ = $1; }
    ;

target_indexing: indexing { $$ = new IndexingTarget($1); }
    ;

target_attr: attr { $$ = new AttrTarget($1); }
    ;

target_id: id { $$ = new IdTarget(@1.begin.line, $1); }
    ;

// // TODO A
// target_multi: "[" target_id "," target_id "]" {
//         $$ = new MultiTarget(@1.begin.line, { $2, $4 });
//     }
//     ;

// "(" • ID "," target_id ")" "=" set
// ( a, b ) = c = 42

// TODO A : Without brackets also
target_multi: "[" target_multi_content "]" {
        $$ = new MultiTarget(@1.begin.line, $2);
    }
    ;

// TODO A : Not only ids
target_multi_content: target_id { $$ = { $1 }; }
    | target_multi_content "," target_id { $$ = $1; $$.push_back($3); }
    ;

// cascade: boolean { $$ = $1; }
// TODO B
//     | cascade option_stop ".." boolean {
//         $$ = new Cascade(@1.begin.line, $1, $4);
//     }
    ;


// TODO B
/*
o..a()
-> (o.a)(); o
*/


/*
cascade: cascade cascade_section
    | boolean cascade_section
    ;

cascade_section: ID cascade_section_tail
    ;

cascade_section_tail: cascade_assignment
    | selector* (assignable_selector cascade_assignment)?
    ;

cascade_assignment: assignment_operator boolean
    ;

selector: assignable_selector
    | args // ex: (a, b, c)
    ;

assignable_selector: "." ID
    ;

// cascade_section: cascade_selector cascade_section_tail
//     ;

// cascade_selector: ID
//     ;
*/



boolean: comparison { $$ = $1; }
    | "not" comparison { $$ = new UnaExp(@1.begin.line, $2, UnaExp::Not); }
    | boolean "or" comparison { $$ = new BinExp(@1.begin.line, $1, BinExp::Or, $3); }
    | boolean "and" comparison { $$ = new BinExp(@1.begin.line, $1, BinExp::And, $3); }
    ;

comparison: binary { $$ = $1; }
    | comparison "==" binary { $$ = new BinExp(@1.begin.line, $1, BinExp::Equal, $3); }
    | comparison "!=" binary { $$ = new BinExp(@1.begin.line, $1, BinExp::NotEqual, $3); }
    | comparison "<=" binary { $$ = new BinExp(@1.begin.line, $1, BinExp::LesserEqual, $3); }
    | comparison ">=" binary { $$ = new BinExp(@1.begin.line, $1, BinExp::GreaterEqual, $3); }
    | comparison "<" binary { $$ = new BinExp(@1.begin.line, $1, BinExp::Lesser, $3); }
    | comparison ">" binary { $$ = new BinExp(@1.begin.line, $1, BinExp::Greater, $3); }
    | comparison "in" binary { $$ = new BinExp(@1.begin.line, $1, BinExp::In, $3); }
    | comparison "not" "in" binary { $$ = new BinExp(@1.begin.line, $1, BinExp::NotIn, $4); }
    | comparison "is" binary { $$ = new BinExp(@1.begin.line, $1, BinExp::Is, $3); }
    | comparison "is" "not" binary { $$ = new BinExp(@1.begin.line, $1, BinExp::IsNot, $4); }
    | comparison "->" binary { $$ = new RangeExp($1, $3, nullptr, false); }
    | comparison "->=" binary { $$ = new RangeExp($1, $3, nullptr, true); }
    | comparison "->" binary ".." binary { $$ = new RangeExp($1, $3, $5, false); }
    | comparison "->=" binary ".." binary { $$ = new RangeExp($1, $3, $5, true); }
    ;

// TODO : Multiple comparisons
// comparison: binary
//     | binary comp_list
// comparison_list: "<" binary
//     | "is" binary
//     | "is" "not" binary

// Binary arithmetic
binary : unary { $$ = $1; }
    | binary "+" unary { $$ = new BinExp(@1.begin.line, $1, BinExp::Add, $3); }
    | binary "-" unary { $$ = new BinExp(@1.begin.line, $1, BinExp::Sub, $3); }
    | binary "*" unary { $$ = new BinExp(@1.begin.line, $1, BinExp::Mul, $3); }
    | binary "/" unary { $$ = new BinExp(@1.begin.line, $1, BinExp::Div, $3); }
    | binary "%" unary { $$ = new BinExp(@1.begin.line, $1, BinExp::Mod, $3); }
    ;

// Unaries
unary: primary { $$ = $1; }
    | "-" primary { $$ = new UnaExp(@1.begin.line, $2, UnaExp::Neg); }
    ;

// Primaries
primary: atom { $$ = $1; }
    | lparen exp rparen { $$ = $2; }
    | call { $$ = $1; }
    | indexing { $$ = $1; }
    | attr { $$ = $1; }
    // TODO B : | cascade { $$ = $1; }
    ;

attr: primary "." ID { $$ = new Attr(@1.begin.line, $1, $3); }
    ;

// TODO B : Handle stops + multiples
// cascade: primary ".." call { $$ = new Cascade(@1.begin.line, $1, $3); }
    // ;

indexing: primary "[" exp "]" { $$ = new Indexing(@1.begin.line, $1, $3); }
    ;

// Call
call: primary lparen call_args rparen {
        $$ = $3;
        $$->fileline = @1.begin.line;
        $$->exp = $1;
    }
    ;

call_args:
    option_stop { $$ = new CallExp(0); }
    | option_stop call_args_filled option_comma_stop { $$ = $2; }
    ;

call_args_filled: ID ":" exp {
        $$ = new CallExp(0);
        $$->kwargs.push_back({ $1, $3 });
    }
    | exp {
        $$ = new CallExp(0);
        $$->args.push_back($1);
    }
    | call_args_filled "," option_stop exp { $$ = $1; $$->args.push_back($4); }
    | call_args_filled "," option_stop ID ":" exp { $$ = $1; $$->kwargs.push_back({ $4, $6 }); }
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
    | map_content_filled option_comma { $$ = $1; }
    ;

map_content_filled: exp ":" exp { $$ = {{ $1, $3 }}; }
    | map_content_filled "," option_stop exp ":" exp { $$ = $1; $$.push_back({ $4, $6 }); }
    ;

// Vec
vec: lbrack vec_content rbrack { $$ = new VecLiteral(@1.begin.line, $2); }
    ;

vec_content: %empty { $$ = {}; }
    | vec_content_filled option_comma { $$ = $1; }
    ;

vec_content_filled: exp { $$ = { $1 }; }
    | vec_content_filled "," option_stop exp { $$ = $1; $$.push_back($4); }
    ;

const: INT { $$ = new Const(@1.begin.line, $1); }
    | STR { $$ = new Const(@1.begin.line, $1); }
    ;

id: ID { $$ = new Id(@1.begin.line, $1); }
    | "(" macro_keyword ")" { $$ = new Id(@1.begin.line, $2); }
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

// Optional tokens
option_stop: %empty
    | stop
    ;

option_comma: %empty
    | "?"
    ;

// ","? stop?
option_comma_stop: %empty
    | stop
    | ","
    | "," stop
    ;
%%

void yy::parser::error(const location_type &l, const std::string &m) {
    driver.error(l.begin.line, l.begin.column, l.end.line, l.end.column, m);
}