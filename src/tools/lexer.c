#include "lexer.h"

int lexer(char *buffer, Token *tokens, Regex **lexicon, TokenType *tokenTypes,
          int tokenTypeCount) {
  int tokenCount = 0;
  char *bp = buffer;
  int STR_LENGTH = 10000;
  char error[STR_LENGTH];
  int e;
  int offset = 0;
  int sizeOfCode = strlen(buffer);
  int execSettings = REG_NEWLINE | REG_NOTEOL;
  int lineCount = 1;

  compileRegex(lexicon, tokenTypeCount);

  int error_count = 0;

  while (offset < sizeOfCode && *bp != '\0') {
    Token token;

    if (*bp == '\0')
      break;

    for (int i = 0; i < tokenTypeCount; i++) {
      Regex *lexiconItem = lexicon[i];
      int subExpr = lexiconItem->r.re_nsub + 1;
      regmatch_t result[subExpr];
      e = regexec(&(lexiconItem->r), bp, subExpr, result, execSettings);

      if (e == 0 && result[0].rm_so == 0) {
        error_count = 0;
        token.type = tokenTypes[i];
        token.content = get_memory(STR_LENGTH);

        copyResult(result[0].rm_so, result[0].rm_eo, bp, token.content);

        log_trace("token[%d] content: %s", tokenCount, token.content);

        if (i == NEWLINE) {
          lineCount++;
        }
        tokens[tokenCount] = token;
        tokenCount++;
        offset += result[0].rm_eo - result[0].rm_so;
        bp = &buffer[offset];
        break;
      } else if (e != 0 && error_count == tokenTypeCount - 1) {
        regerror(e, &lexiconItem->r, error, STR_LENGTH);
        log_error("Lexer error(line %d): %s", lineCount, error);
        log_error("token that did not match: \"%s\"", strndup(bp, subExpr));
        exit(EXIT_FAILURE);
      } else {
        error_count++;
      }
    }
    assert(e == 0);
  }

  freeBuffers(tokens, tokenCount, lexicon, tokenTypeCount);
  return tokenCount;
}

void copyResult(regoff_t sp, regoff_t ep, char *source, char *result) {
  int i = sp;
  while (i < ep) {
    *result++ = source[i++];
  }
  *result = '\0';
}

int compileRegex(Regex **lexicon, int count) {
  int LENGTH = 100;
  char error[LENGTH];
  int e;

  for (int i = 0; i < count; i++) {
    Regex *lexiconItem = lexicon[i];
    e = regcomp(&(lexiconItem->r), lexiconItem->expression, REG_EXTENDED);
    regerror(e, &(lexiconItem->r), error, LENGTH);
    int re_nsub = lexiconItem->r.re_nsub;
    log_trace("lexicon[%d]: %s re_nsub: %d", i,
              e == 0 ? "Sucessfully parsed regex!" : error, re_nsub);
    log_trace("lexicon[%d]: expr: %s", i, lexiconItem->expression);
  }

  return e;
}

void freeBuffers(Token *tokens, int arrIndex, Regex **lexicon, int count) {
  for (int i = 0; i < count; i++) {
    regfree(&(lexicon[i]->r));
  }
  log_info("Lexer buffers free\n");
}
