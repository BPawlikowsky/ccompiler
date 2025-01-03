#include "parser.h"

ParsingTable *loadParsingTable() {
  FILE *fp = fopen("ptable.dat", "rb");
  if (fp == NULL) {
    printf("error: could not open file");
  }

  ParsingTable *ptp = malloc(sizeof(ParsingTable));

  fread(ptp, sizeof(ParsingTable), 1, fp);
  printf("table size: %d\n", ptp->tableSize);
  printf("nonterminal count: %d\n", ptp->nonterminalCount);
  printf("terminal count: %d\n", ptp->terminalCount);

  TableEntry **table = malloc(sizeof(TableEntry) * ptp->tableSize);

  for (int tableEntryIndex = 0; tableEntryIndex < ptp->tableSize;
       tableEntryIndex++) {
    table[tableEntryIndex] = malloc(sizeof(TableEntry));
    fread(table[tableEntryIndex], sizeof(TableEntry), 1, fp);

    if (table[tableEntryIndex]->error == false) {
      table[tableEntryIndex]->production = malloc(sizeof(Production));
      Production *production = table[tableEntryIndex]->production;
      fread(production, sizeof(Production), 1, fp);

      production->statements =
          malloc(sizeof(Statement) * production->statementCount);

      for (int statementIndex = 0; statementIndex < production->statementCount;
           statementIndex++) {
        production->statements[statementIndex] = malloc(sizeof(Statement));
        Statement *statement = production->statements[statementIndex];
        fread(statement, sizeof(Statement), 1, fp);
        statement->content = malloc(sizeof(char) * statement->stringLength);
        fread(statement->content, sizeof(char) * statement->stringLength, 1,
              fp);
      }
    }
  }
  fclose(fp);

  ptp->table = table;

  FILE *file = fopen("nonterminals.txt", "r");
  ptp->nonterminals = malloc(sizeof(char *) * ptp->nonterminalCount);
  for (int i = 0; i < ptp->nonterminalCount; i++) {
    char *result = malloc(sizeof(char) * 30);
    fscanf(file, "%s\n", result);
    ptp->nonterminals[i] = result;
  }
  fclose(file);

  file = fopen("terminals.txt", "r");
  ptp->terminals = malloc(sizeof(char *) * ptp->terminalCount);
  for (int i = 0; i < ptp->terminalCount; i++) {
    char *result = malloc(sizeof(char) * 30);
    fscanf(file, "%s\n", result);
    ptp->terminals[i] = result;
  }
  fclose(file);

  // check
  for (int i = 0; i < ptp->nonterminalCount * ptp->terminalCount; i++) {
    if (table[i]->error == false) {
      Production *prod = table[i]->production;

      printf("%d: Statement count: %d", i, prod->statementCount);

      for (int j = 0; j < prod->statementCount; j++) {
        printf("\"%s\" ", prod->statements[j]->content);
      }
      printf("\n");
    }
  }

  printf("Non-terminals\n");
  for (int i = 0; i < ptp->nonterminalCount; i++) {
    printf("%s\n", ptp->nonterminals[i]);
  }

  printf("Terminals\n");
  for (int i = 0; i < ptp->terminalCount; i++) {
    printf("%s\n", ptp->terminals[i]);
  }

  return ptp;
}

int getNonTerminalIndex(char *nonterminal, ParsingTable *table) {
  for (int i = 0; i < table->nonterminalCount; i++) {
    if (strcmp(nonterminal, table->nonterminals[i]) == 0) {
      return i;
    }
  }
  return -1;
}

int getTerminalIndex(char *terminal, ParsingTable *table) {
  for (int i = 0; i < table->terminalCount; i++) {
    if (strcmp(terminal, table->terminals[i]) == 0) {
      return i;
    }
  }
  return -1;
}

int main(void) {
  ParsingTable *parsingTable = loadParsingTable();
  Token tokens[1000];
  char *buffer = (char *)malloc(2000 * sizeof(char));
  loadFileToBuffer("hello.c", buffer);

  int tokenCount = lexer(buffer, tokens, lexicon, tokenTypes, COUNT);
  free(buffer);
  printf("Token count: %d\n", tokenCount);

  char **stack = malloc(sizeof(char *) * tokenCount * tokenCount);
  stack[0] = "$";
  stack[1] = "start";

  int stackCounter = 2;
  int tokenCounter = 0;

  Token *endToken = malloc(sizeof(Token));
  endToken->content = "$";
  endToken->type = COUNT;
  tokens[tokenCount++] = *endToken;

  while (stackCounter > 0) {
    printf("\n\n");
    printf("stackCounter: %d\n", stackCounter);
    int currentStackIndex = stackCounter - 1;
    char *topStackItem = stack[currentStackIndex];
    TokenType topItemType = getItemTokenType(topStackItem, parsingTable);
    Token token = tokens[tokenCounter];
    char *tokenString = token.content;

    printf("Parsing token no. %d: \"%s\"\n", tokenCounter, token.content);

    if (token.type == IDENTIFIER) {
      tokenString = "identifier";
    }

    if (token.type == MULTILINE_COMMENT || token.type == SINGLELINE_COMMENT ||
        token.type == PREPROCESSOR || token.type == NEWLINE ||
        token.type == WHITESPACE) {
      printf("Ignoring token of type %s\n", tokenTypeStrings[token.type]);
      tokenCounter++;
      continue;
    }

    switch (topItemType) {
    case NONTERMINAL: {
      printf("case NONTERMINAL\n");

      printf("row: \"%s\", col: \"%s\"\n", stack[currentStackIndex],
             tokenString);
      int col = getTerminalIndex(tokenString, parsingTable);
      int row = getNonTerminalIndex(stack[currentStackIndex], parsingTable);

      printf("token type: %s, token content: \"%s\", row: %d, col: %d\n",
             tokenTypeStrings[token.type], tokenString, row, col);
      printf("Current stack item: %s\n", stack[currentStackIndex]);

      int tableIndex = (row * parsingTable->terminalCount) + col;
      printf("tableIndex: (%d * %d) + %d = %d\n", row,
             parsingTable->terminalCount, col, tableIndex);
      TableEntry *entry = parsingTable->table[tableIndex];

      printf("entry no. %d, { error: %s }\n", tableIndex,
             entry->error == true ? "true" : "false");

      if (entry->error == false) {
        printf("Starting to add items to stack...\n");
        printf("statementCount: %d\n", entry->production->statementCount);

        /* POP STUFF OFF THE STACK */
        stackCounter--;

        Production *prod = entry->production;
        for (int i = prod->statementCount - 1; i >= 0; i--) {
          // for (int i = 0; i < prod->statementCount; i++) {
          Statement *statement = prod->statements[i];

          if (strcmp(statement->content, "epsilon") == 0) {
            continue;
          }

          printf("Pushing \"%s\" to stack.\n", statement->content);

          stack[stackCounter++] = statement->content;
        }
      } else {
        printf("error\n");
        return 0;
      }
    }; break;
    case TERMINAL: {
      printf("case TERMINAL\n");
      printf("Comparing \"%s\" to \"%s\"\n", tokenString,
             stack[currentStackIndex]);
      if (strcmp(tokenString, stack[currentStackIndex]) == 0) {
        printf("popping stack.\n");
        /* POP STUFF OFF THE STACK */
        stackCounter--;
        if (tokenCounter < tokenCount - 1) {
          tokenCounter++;
        }
      } else {
        printf("error\n");
        return 0;
      }
    } break;

    default:
      break;
    }
  }

  printf("Stack counter at %d\n", stackCounter);
  return 0;
}

int getItemTokenType(char *item, ParsingTable *table) {
  for (int i = 0; i < table->nonterminalCount; i++) {
    if (strcmp(item, table->nonterminals[i]) == 0) {
      return NONTERMINAL;
    }
  }
  for (int i = 0; i < table->terminalCount; i++) {
    if (strcmp(item, table->terminals[i]) == 0) {
      return TERMINAL;
    }
  }
  return -1;
}