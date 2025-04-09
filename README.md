# 🐝 Bee

Bee is a compiled language.

## Grammar

```ebnf
prog           ::= stmt*
stmt           ::= exit_stmt | var_decl_stmt
expr           ::= int_lit | identifier
type_anotation ::= ":" type
var_decl_stmt  ::= "let" identifier type_anotation? "=" expr ";"
exit_stmt      ::= "exit" expr ";"
```
## TODO
- [] Implement type annotation parsing in var declaration;
- [] Implement constant variable declaration parsing;
- [] Implement support to more literal types;
- [] Implement code generation for expression and var declaration statements;
- [] Implement type checker and symbol table;
- [] Create a logger for better error handling;

