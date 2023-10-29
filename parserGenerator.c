/*
* E -> DD'
* D -> NP'
* D'-> D | eps
* P'-> PP' | eps
* P -> N'
* N'-> NN' | eps
* N -> strS
* S -> : | CR
*
*     | str   :   CR    $
* -------------------------
*   E | DD'       
*   D | NP'
*   D'| D              eps
*   P | N'
*   P'| PP'            eps
*   N | strS
*   N'| NN'            eps
*   S |       :   CR
* */
#include "parserGenerator.h"

char *parseTable[] = {
  "DD'",  NULL, NULL, NULL,
  "NP'",  NULL, NULL, NULL,
  "D",    NULL, NULL, "eps",
  "N'",   NULL, NULL, NULL,
  "PP'",  NULL, NULL, "eps",
  "strS", NULL, NULL, NULL,
  "NN'",  NULL, NULL, "eps",
  NULL,   ":",  "CR", NULL
};

void loadFileToBuffer(char *path, char *bp);
void printTokens(Token tokens[], int it);
void trimProdStrings(Token tokens[], int it);


int main() {
  Token tokens[1000]; 
  char *buffer = (char *)malloc(20000 * sizeof(char));
  loadFileToBuffer("./cgrammer.txt", buffer);

  int tokenCount = lexer(buffer, tokens, lexicon, tokenTypes, COUNT);
  printf("Token count: %d\n", tokenCount);
  int it = 0;
  int defCount = 0;
  int prodCount = 0;
  while(it < tokenCount) {
    printTokens(tokens, it);
    if(tokens[it].type == PROD) prodCount++;
    if(tokens[it].type == DEF) defCount++;
    it++;
  }
  printf("Definitions: %d\n", defCount);
  printf("Productions: %d\n", prodCount);

  char *defs[defCount];
  it = 0;
  int id = 0;
  while(it < tokenCount) {
    if(tokens[it].type == DEF) {
      char *fp = strstr(tokens[it].content, ":");
      tokens[it].content[strlen(tokens[it].content) - 1] = '\0';
      defs[id] = tokens[it].content;
      printTokens(tokens, it);
      id++;
    }
    if(tokens[it].type == PROD) {
      trimProdStrings(tokens, it);
      printf("After trim: %s\n", tokens[it].content);
    }
    it++;
  }

  return 0;
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

void printTokens(Token tokens[], int it) {
  printf("%d: %s %s\n", it, tokens[it].content, typeToString(tokens[it].type));
}

void trimProdStrings(Token tokens[], int it) {
  tokens[it].content = tokens[it].content + 3;
  char *fp = strstr(tokens[it].content, "</p>");
  tokens[it].content[fp - tokens[it].content] = '\0';
}
