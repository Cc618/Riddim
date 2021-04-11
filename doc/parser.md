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
