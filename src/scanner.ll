%{
# include <cerrno>
# include <climits>
# include <cstdlib>
# include <string>
# include "driver.hh"
# include "parser.hh"
# include "ast.hh"
# include "tokens.hh"
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

id    [a-zA-Z][a-zA-Z_0-9]*
int   [0-9]+
str   '[^']*'
blank [ \t\r]
/* TODO */
comment #.*$

%{
    // Every time a pattern is matched
    #define YY_USER_ACTION loc.columns(yyleng);
%}
%%
%{
    // Every time yylex is called.
    yy::location& loc = drv.location;
    loc.step();
%}
{blank}+        loc.step();
\n+             loc.lines(yyleng);loc.step ();
{comment}       loc.step();

"+"             return yy::parser::make_PLUS(loc);
"("             return yy::parser::make_LPAREN(loc);
")"             return yy::parser::make_RPAREN(loc);
"{"             return yy::parser::make_LBLOCK(loc);
"}"             return yy::parser::make_RBLOCK(loc);
"="             return yy::parser::make_EQ(loc);
";"             return yy::parser::make_STOP(loc);

{int}           return make_INT(yytext, loc);
{str}           return make_STR(yytext, loc);
{id}            return yy::parser::make_ID(yytext, loc);
.                    {
    throw yy::parser::syntax_error
        (loc, "invalid character: " + std::string(yytext));
}
<<EOF>>         return yy::parser::make_YYEOF(loc);
%%
