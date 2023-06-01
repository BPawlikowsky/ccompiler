#include <stdlib.h>
#include "lexer.h"

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

  lexer(buffer, tokens, arrIndex);

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
