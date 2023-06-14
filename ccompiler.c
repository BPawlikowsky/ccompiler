#include <stdlib.h>
#include "lexer.h"

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

typedef struct {
  struct Node *children;
  int childrenCount;
  Token *tokens;
  int tokenCount;
} Node;

void loadFileToBuffer(char *path, char *bp);

int main(void) {
  regmatch_t result[1];
  Token tokens[100];
  int arrIndex = 0;
  char *buffer = (char *)malloc(1000 * sizeof(char));

  loadFileToBuffer("./hello.c", buffer);
  printf("File closed\n");

  // Print file
  printf("------------------------------\n");
  printf("%s\n", buffer);
  printf("------------------------------\n");

  lexer(buffer, tokens, arrIndex, lexicon, tokenTypes);

  free(buffer);

  return 0;
}

void parser(Token *tokens, int tokenCount) {
  int cursor = 0;
  Token *tp = tokens;
  while(cursor < tokenCount) {
  }
}

void loadFileToBuffer(char *path, char *bp) {
  FILE *fp;
  if((fp = fopen(path, "r")) == NULL)
    printf("Could not open file\n");
  else {
    while((*bp++ = fgetc(fp)) != EOF) {}
    *--bp = '\0';
  }
  fclose(fp);
}
