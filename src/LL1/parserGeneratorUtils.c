#include "parserGeneratorUtils.h"

int getTerminalIndexFromIndex(ParsingTable *table, char *name) {
  int terminalCount = table->terminalCount;
  char **terminals = table->terminals;

  for (int i = 0; i < terminalCount; i++) {
    if (strcmp(terminals[i], name) == 0) {
      return i;
    }
  }

  return -1;
}

int getNonTerminalIndexFromIndex(ParsingTable *table, char *name) {
  int nonterminalCount = table->nonterminalCount;
  char **nonterminals = table->nonterminals;

  for (int i = 0; i < nonterminalCount; i++) {
    if (strcmp(nonterminals[i], name) == 0) {
      return i;
    }
  }

  return -1;
}
