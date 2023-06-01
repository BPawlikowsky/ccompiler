#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <../../include/regex.h>

typedef enum TokenType {
  PREPROCESSOR = 0,
  IDENTIFIER,
  NUMBER,
  KEYWORD,
  WHITESPACE,
  NEWLINE,
  PUNCTUATION,
  COUNT
} TokenType;

typedef struct {
  regex_t r;
  char *expression;
} Regex;

typedef struct {
  TokenType type;
  char *content;
} Token;

void lexer(char *buffer,Token *tokens, int arrIndex);
