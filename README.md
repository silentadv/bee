# 🐝 Bee

Bee is a compiled language.

## Grammar

```ebnf
prog       ::= stmt*
stmt       ::= exit_stmt
exit_stmt  ::= "exit" int_lit ";"
```
