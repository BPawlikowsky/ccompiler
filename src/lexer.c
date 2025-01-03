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
  int execSettings = REG_NOTBOL | REG_NOTEOL;
  int lineCount = 1;

  compileRegex(lexicon, tokenTypeCount);

  while (offset < sizeOfCode && *bp != '\0') {
    Token token;
    char *line = malloc(160 * sizeof(char));

    // while (*bp == '\n') {
    //   bp++;
    //   offset++;
    //   lineCount++;
    // }
    if (*bp == '\0')
      break;
    // getCodeLine(bp, line);

    // printf("line no%d: %s | length: %d\n", lineCount, line,
    // (int)strlen(line));

    for (int i = 0; i < tokenTypeCount; i++) {
      Regex *lexiconItem = lexicon[i];
      int subExpr = lexiconItem->r.re_nsub + 1;
      regmatch_t result[subExpr];
      if ((e = regexec(&(lexiconItem->r), bp, subExpr, result, execSettings)) ==
              0 &&
          result[0].rm_so == 0) {
        token.type = tokenTypes[i];
        token.content = malloc(STR_LENGTH);
        copyResult(result[0].rm_so, result[0].rm_eo, bp, token.content);

        printf("token[%d] content: %s\n", tokenCount, token.content);

        tokens[tokenCount] = token;
        tokenCount++;
        offset += result[0].rm_eo - result[0].rm_so;
        bp = &buffer[offset];
        break;
      } else {
        regerror(e, &lexiconItem->r, error, STR_LENGTH);
        // printf("Regex-%d execution: %s\n", tokenCount, error);
        // printf("Line: %d | Offset: %d / %d\n", lineCount + 1, offset,
        //  sizeOfCode);
        // getchar();
      }
    }
    assert(e == 0);
    // printf("-------------------------------------\n");
    free(line);
  }

  freeBuffers(tokens, tokenCount, lexicon, tokenTypeCount);
  return tokenCount;
}

void getCodeLine(char *str, char *result) {
  while ((*result++ = *str++) != '\n') {
  }
  result--;
  *result = '\0';
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
    printf("lexicon[%d] compilation: %s re_nsub: %d expr: %s\n", i, error,
           re_nsub, lexiconItem->expression);
  }

  return e;
}

void freeBuffers(Token *tokens, int arrIndex, Regex **lexicon, int count) {
  for (int i = 0; i < count; i++) {
    regfree(&(lexicon[i]->r));
  }
  printf("Lexer buffers free\n");
}
