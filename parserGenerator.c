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
#include <stdlib.h>
#include <string.h>

void loadFileToBuffer(char *path, char *bp);
void printTokens(Token tokens[], int it);
void trimProdStrings(Token tokens[], int it);
void addStatements(Production *prod, Token tokens[], int it);


int main() {
  Token tokens[1000]; 
  char *buffer = (char *)malloc(20000 * sizeof(char));
  loadFileToBuffer("./cgrammer.txt", buffer);

  int tokenCount = lexer(buffer, tokens, lexicon, tokenTypes, COUNT);
  free(buffer);
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
  int ip = 0;
  Definition *definitions = (Definition *)malloc(sizeof(Definition) * defCount);
  while(it < tokenCount) {
    printTokens(tokens, it);
    if(tokens[it].type == DEF) {
      printf("Start def\n");
      // Trim definition string
      char *fp = strstr(tokens[it].content, ":");
      tokens[it].content[fp - tokens[it].content] = '\0';
      // Add def to definitions array
      defs[id] = tokens[it].content;
      definitions[id].productionCount = 0;
      ip = 0;
      id++;
    }
    if(tokens[it].type == PROD) {
      printf("Start prod\n");
      Production *prod = (Production *)malloc(sizeof(Production));
      printf("Control prod after malloc\n");
      trimProdStrings(tokens, it);
      prod->statementCount = 0;
      definitions[id].productions[ip] = prod;
      addStatements(definitions[id].productions[ip], tokens, it);
      definitions[id].productionCount++;
      ip++;
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

void addStatements(Production *prod, Token tokens[], int it) {
  char *p = tokens[it].content;
  char *statementsLeft = tokens[it].content;
  printf("Token: %s\n", tokens[it].content);
  char *finish = &tokens[it].content[strlen(tokens[it].content)];
  int is = 0;
  while( p < finish) {
    printf("iteration %d\n", is);
    Statement *statement = (Statement *)malloc(sizeof(Statement));
    assert(statement != NULL);
    char *statementEnd = strstr(statementsLeft, " ");
    if(statementEnd == NULL) {
      printf("\tCHECK: strlen(%d)\n", strlen(statementsLeft));
      statement->content = malloc(sizeof(char) * strlen(statementsLeft));
      assert(statement->content != NULL);
      strcpy(statement->content, statementsLeft);
      statement->content[strlen(statementsLeft)] = '\0';
      prod->statements[is++] = statement;
      break;
    }
    long offset = statementEnd - p;
    printf("\toffset: %d\n", offset);

    statement->content = malloc(sizeof(char) * offset);
    assert(statement->content != NULL);
    strncpy(statement->content, statementsLeft, offset);
    statement->content[offset] = '\0';
    statementsLeft += offset + 1;
    p += offset + 1;
    prod->statements[is++] = statement;
  }
  prod->statementCount = is;
  printf("\tStatements added: %s\n", tokens[it].content);
}

