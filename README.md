# 🐝 Bee

Bee is a compiled language.

## Grammar

```ebnf
prog           ::= stmt*
stmt           ::= expr_stmt | var_decl_stmt
expr           ::= literal | identifier | call
call           ::= identifier "!" "(" arguments? ")" ";"
arguments      ::= expr ( "," expr)*
literal        ::= integer | string
type_anotation ::= ":" type
expr_stmt      ::= expr ";"
var_decl_stmt  ::= "let" identifier type_anotation? "=" expr ";"
```

## Examples

```ts
let number: int = 10;
let exitCode = number;
let str = "Hello, World!";
let message: str = str;
let messageLen = strlen!(message);

print!(message);
exit!(exitCode);
```
