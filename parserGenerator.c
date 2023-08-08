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


int main() {
  Token tokens[1000]; 
  char *buffer = (char *)malloc(20000 * sizeof(char));
  loadFileToBuffer("./cgrammer.txt", buffer);

  int tokenCount = lexer(buffer, tokens, lexicon, tokenTypes, COUNT);
  printf("Token count: %d\n", tokenCount);
  int it = 0;
  while(it < tokenCount) {
    printf("%d: %s %s\n", it, tokens[it].content, typeToString(tokens[it].type));
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
