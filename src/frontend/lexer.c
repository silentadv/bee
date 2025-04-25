#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../common/token.h"
#include "../common/vector.h"
#include "lexer.h"

static bool __isalnum_predicate(const char c) { return (bool)isalnum(c); }
static bool __isdigit_predicate(const char c) { return (bool)isdigit(c); }
static bool __isstr_predicate(const char c) { return c != '"'; }
static bool __isspace_predicate(const char c) { return (bool)isspace(c); }

lexer_t *lexer_read(const char *filename) {
  FILE *file = fopen(filename, "r");
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

  string_view_t view = sv_create(buffer, size);
  return lexer_new(view);
}

lexer_t *lexer_new(string_view_t source) {
  lexer_t *instance = malloc(sizeof(lexer_t));
  instance->col = 0;
  instance->row = 0;
  instance->cursor = 0;
  instance->source = source;

  return instance;
}

void lexer_destroy(lexer_t *lexer) { free(lexer); }

vector_t *lexer_lex(lexer_t *lexer) {
  const size_t initial_capacity = 25;
  vector_t *tokens = vector_new(initial_capacity, sizeof(token_t));

  lexer->cursor = 0;

  while (lexer->cursor < lexer->source.len) {
    token_t current = lexer_tokenize(lexer);
    vector_push(tokens, &current);
  }

  return tokens;
}

token_t lexer_tokenize(lexer_t *lexer) {
  string_view_t source = sv_slice(lexer->source, lexer->cursor);
  string_view_t space = sv_copy_with_predicate(source, __isspace_predicate);
  lexer->cursor += space.len;

  string_view_t block = sv_slice(source, space.len);
  if (block.len <= 0)
    return (token_t){.kind = TOK_EOF, .lexeme = "EOF"};

  string_view_t number = sv_copy_with_predicate(block, __isdigit_predicate);
  if (number.len > 0) { // tokenizing number token
    lexer->cursor += number.len;
    return (token_t){.kind = TOK_NUM, .lexeme = number};
  }

  string_view_t text = sv_copy_with_predicate(block, __isalnum_predicate);
  if (text.len > 0) { // tokenizing identifiers and keywords
    lexer->cursor += text.len;

    if (sv_equals(text, SV("exit")))
      return (token_t){.kind = TOK_KW_EXIT, .lexeme = text};
    if (sv_equals(text, SV("int")))
      return (token_t){.kind = TOK_KW_INT, .lexeme = text};
    if (sv_equals(text, SV("str")))
      return (token_t){.kind = TOK_KW_STR, .lexeme = text};
    if (sv_equals(text, SV("let")))
      return (token_t){.kind = TOK_KW_LET, .lexeme = text};
    if (sv_equals(text, SV("write")))
      return (token_t){.kind = TOK_KW_WRITE, .lexeme = text};

    return (token_t){.kind = TOK_IDENT, .lexeme = text};
  }

  if (sv_starts_with(block, SV("\""))) { // tokenizing string token
    string_view_t content = sv_copy_with_predicate(
        sv_slice(block, 1),
        __isstr_predicate); // getting the content inside the "

    sv_shift(&block, content.len + 1); // going to end of the string
    if (!sv_starts_with(block, SV("\""))) {
      fprintf(stderr, "lexer -> expected end of string at %zu.\n",
              lexer->cursor);
      exit(EXIT_FAILURE);
    }

    lexer->cursor += content.len + 2;
    return (token_t){.kind = TOK_STR, .lexeme = content};
  }

  lexer->cursor += 1;
  if (sv_starts_with(block, SV(":")))
    return (token_t){.kind = TOK_COLON, .lexeme = SV(":")};
  if (sv_starts_with(block, SV(";")))
    return (token_t){.kind = TOK_SEMICOL, .lexeme = SV(";")};
  if (sv_starts_with(block, SV("=")))
    return (token_t){.kind = TOK_ASSIGN, .lexeme = SV("=")};

  fprintf(stderr, "lexer -> unexpected token found " SV_FMT "\n",
          SV_ARG(sv_rslice(block, 1)));
  exit(EXIT_FAILURE);
}
