#include "lexer.h"


int compileRegex();
void copyResult(regoff_t sp, regoff_t ep, char *source, char *result);
void freeBuffers(Token *tokens, int arrIndex);

Regex preprocRegex    = { 0, .expression = "#[[:print:]]+" };
Regex identifierRegex = { 0, .expression = "[[:alpha:]][[:alnum:]]*" };
Regex numberRegex     = { 0, .expression = "[[:digit:]]+" };
Regex keywordRegex    = { 0, .expression = "auto|break|case|char|const|continue|default|do|double|else|enum|extern|float|for|goto|if|inline|int|long|register|restrict|return|short|signed|sizeof|static|struct|switch|typedef|union|unsigned|void|volatile|while|_Bool|_Complex|_Imaginary" };
Regex whitepsaceRegex = { 0, .expression = "[ \t]" };
Regex newlineRegex    = { 0, .expression = "\n" };
Regex punctRegex    = { 0, .expression = "[(){};\"\"=,]" };

TokenType tokenTypes[] = {
  PREPROCESSOR,
  IDENTIFIER,
  NUMBER,
  KEYWORD,
  NEWLINE,
  PUNCTUATION,
  WHITESPACE
};

Regex *lexicon[] = {
  &preprocRegex,
  &identifierRegex,
  &numberRegex,
  &keywordRegex,
  &newlineRegex,
  &punctRegex,
  &whitepsaceRegex
};

void lexer(char *buffer,Token *tokens, int arrIndex) {
  regmatch_t result[1];
  char *bp = buffer;
  int STR_LENGTH = 100;
  char error[STR_LENGTH];
  int e;
  int offset = 0;
  int sizeOfCode = strlen(buffer);
  int tokenCount = 0;
  int execSettings = REG_NOTBOL;

  compileRegex();

  while(offset < sizeOfCode && arrIndex <= offset && *bp != '\0') {
    Token token;
    for(int i = 0; i < COUNT; i++) {
      if((e = regexec(&(lexicon[i]->r), bp, 1, result, execSettings)) == 0 && result[0].rm_so == 0) {
        token.type = tokenTypes[i];
        token.content = malloc(STR_LENGTH);
        copyResult(result[0].rm_so, result[0].rm_eo, bp, token.content);
        break;
      }
    }

    if(e == 0) {
      tokens[arrIndex] = token;
      arrIndex++;
      offset += result[0].rm_eo - result[0].rm_so;
      bp = &buffer[offset];
    } else {
      regerror(e, &preprocRegex.r, error, STR_LENGTH);
      printf("Regex-%d execution: %s\n",arrIndex, error);
      printf("SO: %d, SE: %d | Offset: %d\n",result[0].rm_so, result[0].rm_eo, offset);
      assert(e == 0);
    }
  }

  // for(int i = 0; i < arrIndex; i++)
  //   printf("<type: %d, content: \'%s\'>\n",tokens[i].type, (tokens[i].type != NEWLINE) ? tokens[i].content : "\\n");
  freeBuffers(tokens, arrIndex);
}

void copyResult(regoff_t sp, regoff_t ep, char *source, char *result) {
  int i = sp;
  while(i < ep) {
    *result++ = source[i++];
  }
  *result = '\0';
}

int compileRegex() {
  int LENGTH = 100;
  char error[LENGTH];
  int e;

  for(int i = 0; i < COUNT; i++) {
    e = regcomp(&(lexicon[i]->r), lexicon[i]->expression, REG_EXTENDED);
    regerror(e, &(lexicon[i]->r), error, LENGTH);
    printf("lexicon[%d] compilation: %s\n", i, error);
  }

  return e;
}

void freeBuffers(Token *tokens, int arrIndex) {
  for(int i = 0; i < COUNT; i++) {
    regfree(&(lexicon[i]->r));
  }

  for(int i = 0; i < arrIndex; i++) {
    free(tokens[i].content);
  }

  printf("Lexer buffers free\n");
}
