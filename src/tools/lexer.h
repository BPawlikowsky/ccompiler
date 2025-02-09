#pragma once

#include "../types.h"
#include "logger.h"
#include "../memory.h"

int lexer(char *buffer, Token *tokens, Regex **lexicon, TokenType *tokenTypes,
          int tokenTypeCount);

int compileRegex(Regex **lexicon, int count);
void copyResult(regoff_t sp, regoff_t ep, char *source, char *result);
void freeBuffers(Token *tokens, int arrIndex, Regex **lexicon, int count);
void getCodeLine(char *str, char *result);
