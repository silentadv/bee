#include "../frontend/parser.h"
#include "../frontend/checker.h"

#include "generator.h"
#include <stdio.h>
#include <stdlib.h>

generator_t* generator_new(prog_t *prog, hashmap_t *symbols) {
    generator_t *instance = malloc(sizeof(generator_t));
    instance->prog = prog;
    instance->symbols = symbols;
    instance->cursor = 0;
    instance->stack_size = 0;
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
        case STMT_VAR_DECL:
            generator_gen_var_decl_stmt(generator);
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

    generator_gen_expr(generator, &exit_stmt.exit_code_expr);

    FILE* output = generator->output;
    fprintf(output, "   mov rdi, rax\n");
    fprintf(output, "   mov rax, 60\n");
    fprintf(output, "   syscall\n");
}

void generator_gen_var_decl_stmt(generator_t *generator) {
    stmt_t *stmt = generator_expect(generator, STMT_VAR_DECL);
    var_decl_stmt_t var_decl_stmt = stmt->body.var_decl_stmt;

    generator_gen_expr(generator, &var_decl_stmt.value);
    
    const char *ident = var_decl_stmt.identifier.identifier.lexeme;
    hashmap_bucket_t *bucket = hashmap_get(generator->symbols, ident);
    symbol_t *symbol = (symbol_t*) bucket->buf;
    symbol->stack_pos = generator->stack_size++;

    FILE* output = generator->output;
    fprintf(output, "   push rax\n");
}

void generator_gen_expr(generator_t *generator, expr_t *expr) {
    switch (expr->kind) {
        case EXPR_LIT:
            generator_gen_lit_expr(generator, &expr->body.lit_expr);
            break;
        case EXPR_IDENT:
            generator_gen_ident_expr(generator, &expr->body.ident_expr);
            break;
        default:
            printf("generator -> Unexpected expression found during generation, received: %d.\n", 
                    expr->kind);
            exit(EXIT_FAILURE);
    }
}

void generator_gen_lit_expr(generator_t *generator, lit_expr_t *expr) {
    FILE* output = generator->output;
    fprintf(output, "   mov rax, %s\n", expr->literal.lexeme);
}

void generator_gen_ident_expr(generator_t *generator, ident_expr_t *expr) {
    const char *ident = expr->identifier.lexeme;
    hashmap_bucket_t *bucket = hashmap_get(generator->symbols, ident);
    
    if (bucket == NULL) {
        printf("generator (unexpected!) -> %s is not defined", ident);
        exit(EXIT_FAILURE);
    }

    symbol_t *symbol = (symbol_t*) bucket->buf;

    FILE* output = generator->output;
    fprintf(output, "   mov rax, [rsp + %lu]\n", 
            (generator->stack_size - symbol->stack_pos - 1) * 8);
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
