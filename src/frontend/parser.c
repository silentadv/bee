#include <stdlib.h>
#include <stdio.h>

#include "parser.h"
#include "../common/vector.h"
#include "../frontend/lexer.h"

parser_t* parser_new(vector_t *tokens) {
    parser_t *instance = malloc(sizeof(parser_t));
    instance->cursor = 0;
    instance->tokens = tokens;

    return instance;
}

prog_t* parser_parse(parser_t *parser) {
    prog_t *prog = malloc(sizeof(prog_t));
    prog->body = vector_new(20, sizeof(stmt_t));

    while (parser_peek(parser) != NULL && parser_peek(parser)->kind != TOK_EOF) {
        token_t *tok = parser_consume(parser);
        
        // parsing exit_stmt
        if (tok->kind == TOK_EXIT) {
            token_t *exit_token = parser_expect(parser, TOK_NUM);
            exit_stmt_t exit_stmt = { .exit_token = *exit_token };
            stmt_t stmt = { 
                .kind = STMT_EXIT, 
                .body = { .exit_stmt = exit_stmt } 
            }; 

            parser_expect(parser, TOK_SEMICOL);

            vector_push(prog->body, &stmt);
            continue;
        }

        printf("Invalid token found during parsing: k: %d -> '%s' \n", 
            tok->kind, tok->lexeme);
        exit(EXIT_FAILURE);
    }

    return prog;
}

token_t* parser_peek(parser_t *parser) {
    if (parser->cursor >= parser->tokens->buf_size) 
        return NULL;

    token_t *tok = vector_at(parser->tokens, parser->cursor);
    return tok;
}

token_t* parser_consume(parser_t *parser) {
    if (parser->cursor >= parser->tokens->buf_size) 
        return NULL;

    token_t *tok = vector_at(parser->tokens, parser->cursor);
    parser->cursor++;

    return tok;
}

token_t* parser_expect(parser_t *parser, token_kind_t kind) {
    if (!parser_peek(parser)) {
        printf("Expected k: %d, but received null.\n", 
            kind);
        exit(EXIT_FAILURE);
    }

    token_t *tok = parser_consume(parser);
    if (tok->kind == kind) return tok;

    if (tok->kind == TOK_EOF) {
        printf("Expected k: %d, but received eof.\n", 
            kind);
        exit(EXIT_FAILURE);
    }

    printf("Expected k: %d, but received: %d.\n", 
        kind, tok->kind);
    exit(EXIT_FAILURE);
}