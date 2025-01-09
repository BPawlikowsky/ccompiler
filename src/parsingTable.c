#include "parsingTable.h"

int totalOverwrites = 0;

ParsingTable *createParsingTable(GeneratorState *state) {
  // table initialization
  ParsingTable *table = malloc(sizeof(ParsingTable));

  table->terminals = state->terminals;
  table->nonterminals = state->nonterminals;
  table->terminalCount = state->terminalCount;
  table->nonterminalCount = state->nonterminalCount;

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
    int row = getNonTerminalIndexFromIndex(table, definition->name);
    assert(row >= 0);
    FirstSet *definition_first_set = state->firstSets[d];
    FollowSet *definition_follow_set = state->followSets[d];

    printf("(%d)definition: %s\n", d, definition->name);

    for (int p = 0; p < definition->productionCount; p++) {
      Production *production = definition->productions[p];
      Statement *firstSymbol = production->statements[0];

      if (production->statementCount == 0) {
        printf("Definition %s has an empty production no. %d\n",
               definition->name, p);
      }

      // IF PRODUCTION IS EPSILON
      if (strcmp(firstSymbol->content, "epsilon") == 0) {
        for (int t = 0; t < state->terminalCount; t++) {
          char *terminalSymbol = state->terminals[t];
          if (isStringInFollowSet(terminalSymbol, definition_follow_set)) {

            int col = getTerminalIndexFromIndex(table, terminalSymbol);
            assert(col >= 0);
            addItemToTable(row, col, production, table);
          }
        }

        for (int t = 0; t < definition_follow_set->itemCount; t++) {
          char *followSymbol = definition_follow_set->set[t];
          int col = getTerminalIndexFromIndex(table, followSymbol);
          assert(col >= 0);
          addItemToTable(row, col, production, table);
        }
      }

      // IF FIRST STATEMENT IS A TERMINAL
      if (firstSymbol->type == TERMINAL &&
          strcmp(firstSymbol->content, "epsilon") != 0) {
        // Add production for this terminal
        int col = getTerminalIndexFromIndex(table, firstSymbol->content);
        assert(col >= 0);
        addItemToTable(row, col, production, table);
      }

      // IF FIRST STATEMENT IS A NONTERMINAL
      if (firstSymbol->type == NONTERMINAL) {
        int firstDefIndex = getDefinitionIndex(state, firstSymbol->content);
        FirstSet *firstSet = state->firstSets[firstDefIndex];

        for (int k = 0; k < firstSet->itemCount; k++) {
          if (strcmp(firstSet->set[k], "epsilon") != 0) {
            int col = getTerminalIndexFromIndex(table, firstSet->set[k]);
            assert(col >= 0);
            addItemToTable(row, col, production, table);
          }
        }

        if (doesSetContainEpsilon(firstSet)) {
          FollowSet *followSet = state->followSets[d];

          for (int m = 0; m < followSet->itemCount; m++) {
            int col = getTerminalIndexFromIndex(table, followSet->set[m]);
            assert(col >= 0);
            addItemToTable(row, col, production, table);
          }
        }
      }
    }
  }

  printf("Total overwrites: %d\n", totalOverwrites);

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

bool isStringInFirstSet(char *str, FirstSet *set) {
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
  }
  int index = (table->terminalCount * row) + col;
  if (table->table[index]->error == false) {
    char *previousProd = productionAsString(table->table[index]->production);
    char *currentProduction = productionAsString(production);

    if (strcmp(previousProd, currentProduction) != 0) {
      log_debug("row: %d, col: %d already filled out", row, col);
      log_debug("previous production: %s",
                productionAsString(table->table[index]->production));
      log_debug("new production: %s", productionAsString(production));
      totalOverwrites++;
    }
  }
  if (row >= 0 && col >= 0 && table->table[index]->error == true) {
    table->table[index]->row = row;
    table->table[index]->col = col;
    table->table[index]->production = production;
    table->table[index]->error = false;
    return true;
  }
  return false;
}

char *productionAsString(Production *prod) {
  int string_size = 0;
  for (int i = 0; i < prod->statementCount; i++) {
    string_size += prod->statements[i]->stringLength;
  }

  char *result = malloc(sizeof(char) * string_size);
  for (int i = 0; i < prod->statementCount; i++) {
    sprintf(result, "%s %s", result, prod->statements[i]->content);
  }
  return result;
}