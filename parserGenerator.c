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

void loadFileToBuffer(char *path, char *bp);
void printTokens(Token tokens[], int it);
void trimProdStrings(Token tokens[], int it);
void addStatements(Production *prod, Token tokens[], int it);
void printDefs(Definition *defs[], int defCount);
StatementType getStatementType(char *defs[], char *statement, int defCount);
void addTypeToStatements(Definition *defs[], int defCount, char *strDefs[]);
Definition *definitions[300];


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
  while(it < tokenCount) {
    printTokens(tokens, it);
    if(tokens[it].type == DEF) {
      printf("Start def\n");
      // Trim definition string
      char *fp = strstr(tokens[it].content, ":");
      tokens[it].content[fp - tokens[it].content] = '\0';
      // Add def to definitions array
      defs[id] = tokens[it].content;
      Definition *definition = (Definition *) malloc(sizeof(Definition));
      definition->name = tokens[it].content;
      definition->productionCount = 0;
      definitions[id] = definition;
      ip = 0;
      id++;
    }
    if(tokens[it].type == PROD) {
      printf("Start prod\n");
      Production *prod = (Production *)malloc(sizeof(Production));
      trimProdStrings(tokens, it);
      prod->statementCount = 0;
      definitions[id - 1]->productions[ip] = prod;
      addStatements(definitions[id - 1]->productions[ip], tokens, it);
      definitions[id - 1]->productionCount++;
      ip++;
    }
    it++;
  }

  addTypeToStatements(definitions, defCount, defs);

  printDefs(definitions, defCount);

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
  char *content = tokens[it].content;
  int type = tokens[it].type;
  printf("%d: %s %s\n", it, content, typeToString(type));
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
      int strLen = strlen(statementsLeft);
      statement->content = (char *)malloc(sizeof(char) * strLen + 1);
      assert(statement->content != NULL);

      strcpy(statement->content, statementsLeft);
      statement->content[strlen(statementsLeft)] = '\0';

      // add single statement to production and break
      prod->statements[is++] = statement;
      break;
    }

    long offset = statementEnd - p;
    printf("\toffset: %ld\n", offset);

    statement->content = (char *)malloc(sizeof(char) * (int)offset + 1);
    assert(statement->content != NULL);

    strncpy(statement->content, statementsLeft, offset);
    statement->content[offset] = '\0';
    // offset + 1 because of the null terminator
    statementsLeft += offset + 1;
    p += offset + 1;
    // add statement to production
    prod->statements[is++] = statement;
  }
  prod->statementCount = is;
  printf("\tStatements added: %s\n", tokens[it].content);
}

void printDefs(Definition *defs[], int defCount) {
  int i = 0;
  while(i < defCount) {
    printf("%d: \"%s\" [\n", i, defs[i]->name);
    int p = 0;
    while(p < defs[i]->productionCount) {
      printf("  [\n");
      int s = 0;
      while(s < defs[i]->productions[p]->statementCount) {
        if(s == defs[i]->productions[p]->statementCount - 1)
          printf("    \"%s\"(%s) \n", defs[i]->productions[p]->statements[s]->content, defs[i]->productions[p]->statements[s]->type == NONTERMINAL ? "NON-TERMINAL" : "TERMINAL");
        else
          printf("    \"%s\"(%s), \n", defs[i]->productions[p]->statements[s]->content, defs[i]->productions[p]->statements[s]->type == NONTERMINAL ? "NON-TERMINAL" : "TERMINAL");
        s++;
      }
      if(p == defs[i]->productionCount - 1)
        printf("  ]\n");
      else
        printf("  ],\n");
      p++;
    }
    printf("]\n");

    i++;
  }
}

StatementType getStatementType(char *defs[], char *statement, int defCount) {
  for(int i = 0; i < defCount; i++) {
    if(strcmp(defs[i], statement) == 0) return NONTERMINAL;
  }
  return TERMINAL;
}

void addTypeToStatements(Definition *defs[], int defCount, char *strDefs[]) {

  int i = 0;
  while(i < defCount) {
    int p = 0;
    while(p < defs[i]->productionCount) {
      int s = 0;
      while(s < defs[i]->productions[p]->statementCount) {
        Statement *statement = defs[i]->productions[p]->statements[s];
        statement->type = getStatementType(strDefs, statement->content, defCount);
        s++;
      }
      p++;
    }
    i++;
  }
}
