#include "lexer.h"


int compileRegex(Regex **lexicon, int count);
void copyResult(regoff_t sp, regoff_t ep, char *source, char *result);
void freeBuffers(Token *tokens, int arrIndex, Regex **lexicon, int count);


int lexer(char *buffer,Token *tokens, Regex **lexicon, TokenType *tokenTypes, int tokenTypeCount) {
  int tokenCount = 0;
  regmatch_t result[1];
  char *bp = buffer;
  int STR_LENGTH = 100;
  char error[STR_LENGTH];
  int e;
  int offset = 0;
  int sizeOfCode = strlen(buffer);
  int execSettings = REG_NOTBOL;

  compileRegex(lexicon, tokenTypeCount);

  while(offset < sizeOfCode && tokenCount <= offset && *bp != '\0') {
    Token token;
    for(int i = 0; i < tokenTypeCount; i++) {
      if((e = regexec(&(lexicon[i]->r), bp, 1, result, execSettings)) == 0 && result[0].rm_so == 0) {
        token.type = tokenTypes[i];
        token.content = malloc(STR_LENGTH);
        copyResult(result[0].rm_so, result[0].rm_eo, bp, token.content);
        break;
      }
    }

    if(e == 0) {
      tokens[tokenCount] = token;
      tokenCount++;
      offset += result[0].rm_eo - result[0].rm_so;
      bp = &buffer[offset];
    } else {
      regerror(e, &lexicon[0]->r, error, STR_LENGTH);
      printf("Regex-%d execution: %s\n",tokenCount, error);
      printf("SO: %d, SE: %d | Offset: %d\n",result[0].rm_so, result[0].rm_eo, offset);
      assert(e == 0);
    }
  }

  freeBuffers(tokens, tokenCount, lexicon, tokenTypeCount);
  return tokenCount;
}

void copyResult(regoff_t sp, regoff_t ep, char *source, char *result) {
  int i = sp;
  while(i < ep) {
    *result++ = source[i++];
  }
  *result = '\0';
}

int compileRegex(Regex **lexicon, int count) {
  int LENGTH = 100;
  char error[LENGTH];
  int e;

  for(int i = 0; i < count; i++) {
    e = regcomp(&(lexicon[i]->r), lexicon[i]->expression, REG_EXTENDED);
    regerror(e, &(lexicon[i]->r), error, LENGTH);
    printf("lexicon[%d] compilation: %s\n", i, error);
  }

  return e;
}

void freeBuffers(Token *tokens, int arrIndex, Regex **lexicon, int count) {
  for(int i = 0; i < count; i++) {
    regfree(&(lexicon[i]->r));
  }
  printf("Lexer buffers free\n");
}
