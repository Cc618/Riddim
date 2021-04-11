# Parser / lexer
The parser and the lexer are managed via Bison / Flex.
That is, the parser is LALR(1).

## Lexer
The file containing rules is src/scanner.ll.
To parse tokens, tokens.{cc,hh} files are used (convert int, strings...).
To avoid issues with extra line feeds at the start / the end of the file,
a new line token is added at the start and at the end of the file.
This is done in driver.cc.

## Parser
The file containing the grammar rules is src/parser.yy.
There are multiple sections :
- Declarations : Functions, types, blocks...
- Statements : Instructions, control blocks...
- Expressions : Every operation that produce a value on the TOS (binary / unary operations...)
- Atoms : Basic expressions such as constants
- Tokens : Compound tokens such as stop which is a sequence of STOP (line feeds)

### Expressions
Expressions are complex because of precedence, some expressions must be parsed
before others.
To avoid conflicts, here is the types of expressions :
- exp : Abstract expression used in statements
- set : Assignment, can be chained
- boolean : Not, And, Or...
- comparison : Binary comparison with boolean comparison operators (==...)
    or other operators like is / is not...
- binary : Binary arithmetic operation (+ - / *...)
- unary : Unary arithmetic operation (-x...)
- primary : Contains atoms, calls, attribute references, indexings...

Expressions are parsed in order from primary to exp (primary has a greater precedence than exp).
