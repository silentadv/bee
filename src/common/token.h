#ifndef TOKEN_H
#define TOKEN_H

typedef enum {
    TOK_EOF = 0,   // end of file
    TOK_KW_EXIT,   // exit keyword
    TOK_KW_INT,    // int type keyword
    TOK_KW_LET,    // let keyword
    TOK_IDENT,     // identifier
    TOK_NUM,       // integer number
    TOK_SEMICOL,   // ;
    TOK_COLON,     // : 
    TOK_ASSIGN,    // =
} token_kind_t;

typedef struct {
    token_kind_t kind;
    char *lexeme;
} token_t;

#endif
