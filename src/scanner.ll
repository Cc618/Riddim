%{
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <string>
#include "driver.hh"
#include "parser.hh"
#include "ast.hh"
#include "tokens.hh"
#include "utils.hh"

using namespace ast;
%}

%{
// Disable warnings
#if defined __clang__
# define CLANG_VERSION (__clang_major__ * 100 + __clang_minor__)
#endif

#if defined __GNUC__ && !defined __clang__ && !defined __ICC
# define GCC_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)
#endif

#if defined GCC_VERSION && 600 <= GCC_VERSION
# pragma GCC diagnostic ignored "-Wnull-dereference"
#endif

#if defined CLANG_VERSION && 500 <= CLANG_VERSION
# pragma clang diagnostic ignored "-Wold-style-cast"
# pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#elif defined GCC_VERSION && 407 <= GCC_VERSION
# pragma GCC diagnostic ignored "-Wold-style-cast"
# pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif

#define FLEX_VERSION (YY_FLEX_MAJOR_VERSION * 100 + YY_FLEX_MINOR_VERSION)

#if FLEX_VERSION < 206 && defined CLANG_VERSION
# pragma clang diagnostic ignored "-Wdocumentation"
#endif

#if FLEX_VERSION < 206
# if defined CLANG_VERSION && 600 <= CLANG_VERSION
#    pragma clang diagnostic ignored "-Wdeprecated-register"
# elif defined GCC_VERSION && 700 <= GCC_VERSION
#    pragma GCC diagnostic ignored "-Wregister"
# endif
#endif

#if FLEX_VERSION < 206
# if defined CLANG_VERSION
#    pragma clang diagnostic ignored "-Wconversion"
#    pragma clang diagnostic ignored "-Wdocumentation"
#    pragma clang diagnostic ignored "-Wshorten-64-to-32"
#    pragma clang diagnostic ignored "-Wsign-conversion"
# elif defined GCC_VERSION
#    pragma GCC diagnostic ignored "-Wconversion"
#    pragma GCC diagnostic ignored "-Wsign-conversion"
# endif
#endif
%}

%option noyywrap nounput noinput batch

id          [a-zA-Z_][a-zA-Z_0-9]*
int_dec     [0-9][0-9_']*
int_hex     0[xX][0-9a-fA-F][0-9a-fA-F_']*
int_bin     0[bB][01][01_']*
str_single  '(\\.|[^'])*'
str_double  \"(\\.|[^\"])*\"
str         ({str_single})|({str_double})
blank       [ \t\r]
comment     #.*

%{
    // Every time a pattern is matched
    #define YY_USER_ACTION loc.columns(yyleng);
%}
%%
%{
    // Every time yylex is called
    yy::location& loc = drv.location;
    loc.step();
%}

{blank}+        loc.step();
{comment}       loc.step();

\n+             {
    loc.lines(yyleng);
    return yy::parser::make_STOP(loc);
}

\\\n            {
    loc.lines(1);
    loc.step();
}

{int_dec}       return make_INT(loc, yytext, yytext, 10);
-{int_dec}      return make_INT(loc, yytext, yytext + 1, 10, true);
{int_hex}       return make_INT(loc, yytext, yytext + 2, 16);
{int_bin}       return make_INT(loc, yytext, yytext + 2, 2);
{str}           return make_STR(loc, yytext);

"."             return yy::parser::make_DOT(loc);
","             return yy::parser::make_COMMA(loc);
":"             return yy::parser::make_COLON(loc);
"+"             return yy::parser::make_PLUS(loc);
"-"             return yy::parser::make_MINUS(loc);
"*"             return yy::parser::make_WILDCARD(loc);
"/"             return yy::parser::make_SLASH(loc);
"%"             return yy::parser::make_MOD(loc);
"("             return yy::parser::make_LPAREN(loc);
")"             return yy::parser::make_RPAREN(loc);
"{"             return yy::parser::make_LCURLY(loc);
"}"             return yy::parser::make_RCURLY(loc);
"["             return yy::parser::make_LBRACK(loc);
"]"             return yy::parser::make_RBRACK(loc);
"=="            return yy::parser::make_EQ(loc);
"!="            return yy::parser::make_NEQ(loc);
"="             return yy::parser::make_ASSIGN(loc);
">="            return yy::parser::make_GE(loc);
"<="            return yy::parser::make_LE(loc);
">"             return yy::parser::make_GREATER(loc);
"<"             return yy::parser::make_LESSER(loc);
"and"           return yy::parser::make_AND(loc);
"or"            return yy::parser::make_OR(loc);
"not"           return yy::parser::make_NOT(loc);
"is"            return yy::parser::make_IS(loc);
"in"            return yy::parser::make_IN(loc);

"if"            return yy::parser::make_IF(loc);
"elif"          return yy::parser::make_ELIF(loc);
"else"          return yy::parser::make_ELSE(loc);
"while"         return yy::parser::make_WHILE(loc);
"try"           return yy::parser::make_TRY(loc);
"catch"         return yy::parser::make_CATCH(loc);
"as"            return yy::parser::make_AS(loc);
"rethrow"       return yy::parser::make_RETHROW(loc);
"return"        return yy::parser::make_RETURN(loc);
"fn"            return yy::parser::make_FN(loc);

"true"          return yy::parser::make_TRUE(loc);
"false"         return yy::parser::make_FALSE(loc);
"null"          return yy::parser::make_NULL(loc);

{id}            return yy::parser::make_ID(yytext, loc);

.               {
    lexer_error(loc, str_t("Invalid token: ") + yytext);
}

<<EOF>>         return yy::parser::make_EOF(loc);
%%
