#include "parsingTable.h"

int getTerminals(char **terminals, GeneratorState *state) {
  int counter = 0;
  for (int i = 0; i < state->defCount; i++) {
    Definition *definition = state->definitions[i];
    for (int j = 0; j < definition->productionCount; j++) {
      Production *production = definition->productions[j];
      for (int k = 0; k < production->statementCount; k++) {
        Statement *statement = production->statements[k];
        if (statement->type == TERMINAL &&
            doesStringExistInArray(terminals, counter, statement->content) ==
                false &&
            strcmp(statement->content, "epsilon") != 0) {
          terminals[counter] = statement->content;
          counter++;
        }
      }
    }
  }

  return counter;
}

int getNonTerminals(char **nonterminals, GeneratorState *state) {
  int counter = 0;

  for (int i = 0; i < state->defCount; i++) {
    Definition *definition = state->definitions[i];

    if (doesStringExistInArray(nonterminals, counter, definition->name) ==
        false) {
      nonterminals[counter++] = definition->name;
    }

    for (int j = 0; j < definition->productionCount; j++) {
      Production *production = definition->productions[j];
      for (int k = 0; k < production->statementCount; k++) {
        Statement *statement = production->statements[k];
        if (statement->type == NONTERMINAL &&
            doesStringExistInArray(nonterminals, counter, statement->content) ==
                false) {
          nonterminals[counter++] = statement->content;
        }
      }
    }
  }

  return counter;
}

int getTerminalIndex(ParsingTable *table, char *name) {
  int terminalCount = table->terminalCount;
  char **terminals = table->terminals;

  for (int i = 0; i < terminalCount; i++) {
    if (strcmp(terminals[i], name) == 0) {
      return i;
    }
  }

  return -1;
}

int getNonTerminalIndex(ParsingTable *table, char *name) {
  int nonterminalCount = table->nonterminalCount;
  char **nonterminals = table->nonterminals;

  for (int i = 0; i < nonterminalCount; i++) {
    if (strcmp(nonterminals[i], name) == 0) {
      return i;
    }
  }

  return -1;
}

ParsingTable *createParsingTable(GeneratorState *state) {
  // table initialization
  ParsingTable *table = malloc(sizeof(ParsingTable));
  char **terminals = malloc(sizeof(char *) * 150);
  char **nonterminals = malloc(sizeof(char *) * 150);

  table->terminals = terminals;
  table->nonterminals = nonterminals;
  table->terminalCount = getTerminals(terminals, state);
  table->nonterminalCount = getNonTerminals(nonterminals, state);

  table->tableSize = table->nonterminalCount * table->terminalCount;

  printf("terminals: %d, nonterminals: %d\n", table->terminalCount,
         table->nonterminalCount);

  TableEntry **tab = malloc(sizeof(TableEntry) * table->tableSize);

  for (int i = 0; i < table->tableSize; i++) {
    tab[i] = malloc(sizeof(TableEntry));
    tab[i]->error = true;
  }

  table->table = tab;

  for (int d = 0; d < state->defCount; d++) {
    Definition *definition = state->definitions[d];
    int row = getNonTerminalIndex(table, definition->name);

    for (int p = 0; p < definition->productionCount; p++) {
      Production *production = definition->productions[p];
      Statement *firstSymbol = production->statements[0];

      // IF PRODUCTION IS EPSILON
      if (strcmp(firstSymbol->content, "epsilon") == 0) {
        FollowSet *followSet = state->followSets[d];
        for (int m = 0; m < followSet->itemCount; m++) {
          int col = getTerminalIndex(table, followSet->set[m]);
          addItemToTable(row, col, production, table);
        }
      }

      // IF FIRST STATEMENT IS A TERMINAL
      if (firstSymbol->type == TERMINAL &&
          strcmp(firstSymbol->content, "epsilon") != 0) {
        // Add production for this terminal
        int col = getTerminalIndex(table, firstSymbol->content);
        addItemToTable(row, col, production, table);
      }

      // IF FIRST STATEMENT IS A NONTERMINAL
      if (firstSymbol->type == NONTERMINAL) {
        int firstDefIndex = getDefinitionIndex(state, firstSymbol->content);
        FirstSet *firstSet = state->firstSets[firstDefIndex];

        for (int k = 0; k < firstSet->itemCount; k++) {
          if (strcmp(firstSet->set[k], "epsilon") != 0) {
            int col = getTerminalIndex(table, firstSet->set[k]);
            addItemToTable(row, col, production, table);
          }
        }

        if (doesSetContainEpsilon(firstSet)) {
          FollowSet *followSet = state->followSets[d];

          for (int m = 0; m < followSet->itemCount; m++) {
            int col = getTerminalIndex(table, followSet->set[m]);
            addItemToTable(row, col, production, table);
          }
        }
      }
    }
  }

  return table;
}

bool isStringInFollowSet(char *str, FollowSet *set) {
  for (int i = 0; i < set->itemCount; i++) {
    if (strcmp(set->set[i], str) == 0) {
      return true;
    }
  }

  return false;
}

bool addItemToTable(int row, int col, Production *production,
                    ParsingTable *table) {
  if (row < 0 || col < 0) {
    printf("error in row or col, row=%d, col=%d\n", row, col);
    assert(row >= 0);
    assert(col >= 0);
  }
  int index = (table->terminalCount * row) + col;
  if (table->table[index]->error == false) {
    printf("row: %d, col: %d already filled out\n", row, col);
  }
  if (row >= 0 && col >= 0) {
    table->table[index]->row = row;
    table->table[index]->col = col;
    table->table[index]->production = production;
    table->table[index]->error = false;
    return true;
  }
  return false;
}