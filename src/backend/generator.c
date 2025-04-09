#include "../frontend/parser.h"
#include "generator.h"
#include <stdio.h>
#include <stdlib.h>

generator_t* generator_new(prog_t *prog) {
    generator_t *instance = malloc(sizeof(generator_t));
    instance->prog = prog;
    instance->cursor = 0;
    instance->output = NULL;

    return instance;
}

void generator_destroy(generator_t *generator) {
    vector_destroy(generator->prog->body);
    free(generator);
}

void generator_gen(generator_t *generator) {
    generator->cursor = 0;
    generator->output = fopen("./build/out.asm", "w+");

    FILE* output = generator->output;
    if (!output) {
        printf("generator -> Unexpected error ocurred during file opening."); 
        exit(EXIT_FAILURE);
    }

    fprintf(output, "section .text\n");
    fprintf(output, "   global _start\n");
    fprintf(output, "_start:\n");

    while (generator_peek(generator) != NULL)
        generator_gen_stmt(generator);

    fclose(output);
}

void generator_gen_stmt(generator_t *generator) {
    stmt_t *stmt = generator_peek(generator);

    switch (stmt->kind) {
        case STMT_EXIT:
            generator_gen_exit_stmt(generator);
            break;
        default:
            printf("generator -> Unexpected statement found during generation, received: %d.\n", 
                stmt->kind);
            exit(EXIT_FAILURE);
    }
}

void generator_gen_exit_stmt(generator_t *generator) {
    stmt_t *stmt = generator_expect(generator, STMT_EXIT);
    exit_stmt_t exit_stmt = stmt->body.exit_stmt;

    FILE* output = generator->output;
    fprintf(output, "   mov rax, 60\n");
    fprintf(output, "   mov rdi, %s\n", exit_stmt.token.lexeme);
    fprintf(output, "   syscall\n");
}

stmt_t* generator_peek(generator_t *generator) {
    vector_t *stmts = generator->prog->body;
    
    if (generator->cursor >= stmts->buf_size) 
        return NULL;

    stmt_t *stmt = (stmt_t*) vector_at(stmts, generator->cursor);
    return stmt;
}

stmt_t* generator_consume(generator_t *generator) {
    vector_t *stmts = generator->prog->body;
    
    if (generator->cursor >= stmts->buf_size) 
        return NULL;

    stmt_t *stmt = (stmt_t*) vector_at(stmts, generator->cursor);
    generator->cursor++;

    return stmt;
}

stmt_t* generator_expect(generator_t *generator, stmt_kind_t kind) {
    if (!generator_peek(generator)) {
        printf("generator -> Expected k: %d, but received null.\n", 
            kind);
        exit(EXIT_FAILURE);
    }

    stmt_t *stmt = generator_consume(generator);
    if (stmt->kind == kind) return stmt;

    printf("generator -> Expected k: %d, but received: %d.\n", 
        kind, stmt->kind);
    exit(EXIT_FAILURE);
}
