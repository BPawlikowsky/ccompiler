#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include "types.h"

int lexer(char *buffer,Token *tokens, Regex **lexicon, TokenType *tokenTypes, int tokenTypeCount);
