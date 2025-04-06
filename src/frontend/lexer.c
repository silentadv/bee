#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "lexer.h"
#include "../common/vector.h"

lexer_t* lexer_read(const char *filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Invalid file has provided.");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);

    char *buffer = malloc(sizeof(char) * (size + 1));
    if (!buffer) {
        perror("Error ocurred during reading file.");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    fread(buffer, sizeof(char), size, file);
    buffer[size] = '\0';
    
    fclose(file);

    return lexer_new(buffer);
}

lexer_t* lexer_new(const char *source) {
    lexer_t *instance = malloc(sizeof(lexer_t));
    instance->col = 0;
    instance->row = 0;
    instance->cursor = 0;
    instance->source = malloc(strlen(source) + 1);
    strcpy(instance->source, source);

    return instance;
}

vector_t* lexer_lex(lexer_t *lexer) {    
    const size_t initial_capacity = 25;

    vector_t *tokens = vector_new(initial_capacity, sizeof(token_t));

    lexer->cursor = 0;

    while (lexer_peek(lexer).has_value) {
        opt_char_t curr = lexer_consume(lexer);

        if (isalpha(curr.value)) {
            vector_t *lexeme = vector_new(initial_capacity, sizeof(char));
            vector_push(lexeme, &curr.value);

            while(lexer_peek(lexer).has_value && isalnum(lexer_peek(lexer).value)) {
                curr = lexer_consume(lexer);
                vector_push(lexeme, &curr.value);
            }

            vector_push(lexeme, &(char){'\0'});

            if (strcmp(lexeme->buf, "exit") == 0) {
                token_t token = (token_t){ .kind = TOK_EXIT, .lexeme = "exit" }; 
                vector_push(tokens, &token);
                vector_destroy(lexeme);
                continue;
            }

            token_t token = (token_t){ .kind = TOK_IDENT, .lexeme = NULL };
            token.lexeme = malloc(strlen(lexeme->buf) + 1);
            strcpy(token.lexeme, lexeme->buf);

            vector_push(tokens, &token);
            vector_destroy(lexeme);
            continue;
        } 

        if (isdigit(curr.value)) {
            vector_t *lexeme = vector_new(initial_capacity, sizeof(char));
            vector_push(lexeme, &curr.value);

            while(lexer_peek(lexer).has_value && isdigit(lexer_peek(lexer).value)) {
                curr = lexer_consume(lexer);
                vector_push(lexeme, &curr.value);
            }

            vector_push(lexeme, &(char){'\0'});

            token_t token = (token_t){ .kind = TOK_NUM, .lexeme = NULL };
            token.lexeme = malloc(strlen(lexeme->buf) + 1);
            strcpy(token.lexeme, lexeme->buf);

            vector_push(tokens, &token);
            vector_destroy(lexeme);
            continue;
        }
        
        if (curr.value == ';') {
            token_t token = (token_t){ .kind = TOK_SEMICOL, .lexeme = ";" };
            vector_push(tokens, &token);
            continue;
        }

        if (isspace(curr.value))
            continue;

        printf("Invalid character found during lexing: %c \n", curr.value);
        exit(EXIT_FAILURE);
    }

    return tokens;
}

opt_char_t lexer_peek(lexer_t *lexer) {
    size_t lexeme_size = strlen(lexer->source);
    if (lexer->cursor >= lexeme_size) return opt_char_none();

    char c = lexer->source[lexer->cursor];
    return opt_char_some(c);
}

opt_char_t lexer_consume(lexer_t *lexer) {
    size_t lexeme_size = strlen(lexer->source);
    if (lexer->cursor >= lexeme_size) return opt_char_none();

    char c = lexer->source[lexer->cursor++];
    return opt_char_some(c);
}