#include "parserGeneratorUtils.h"

int getDefinitionIndex(GeneratorState *genState, char *statement) {
  for (int i = 0; i < genState->defCount; i++) {
    if (strcmp(genState->defs[i], statement) == 0)
      return i;
  }
  return -1;
}

int doesStringExistInArray(char *history[], int historyCount,
                           char *itemToCheck) {
  for (int i = 0; i < historyCount; i++) {
    if (strcmp(history[i], itemToCheck) == 0)
      return true;
  }
  return false;
}

bool isItemInHistory(GeneratorState *state, char *defName) {
  for (int i = 0; i < state->historyCounter; i++) {
    if (strcmp(state->history[i]->item, defName) == 0) {
      return true;
    }
  }
  return false;
}

bool isItemInHistoryWithFirstSetCreated(GeneratorState *state, char *defName) {
  for (int i = 0; i < state->historyCounter; i++) {
    if (state->history[i]->isFirstSetCreated == true) {
      return true;
    }
  }
  return false;
}

bool isItemInHistoryWithFollowSetCreated(GeneratorState *state, char *defName) {
  for (int i = 0; i < state->historyCounter; i++) {
    if (state->history[i]->isFollowSetCreated == true) {
      return true;
    }
  }
  return false;
}

void printHistory(GeneratorState *state) {
  for (int i = 0; i < state->historyCounter; i++) {
    printf("%d %s\n", i, state->history[i]->item);
  }
}

void printDefs(Definition *defs[], int defCount) {
  int i = 0;
  while (i < defCount) {
    printf("%d: \"%s\" [\n", i, defs[i]->name);
    int p = 0;
    while (p < defs[i]->productionCount) {
      printf("  [\n");
      int s = 0;
      while (s < defs[i]->productions[p]->statementCount) {
        if (s == defs[i]->productions[p]->statementCount - 1)
          printf("    \"%s\"(%s) \n",
                 defs[i]->productions[p]->statements[s]->content,
                 defs[i]->productions[p]->statements[s]->type == NONTERMINAL
                     ? "NON-TERMINAL"
                     : "TERMINAL");
        else
          printf("    \"%s\"(%s), \n",
                 defs[i]->productions[p]->statements[s]->content,
                 defs[i]->productions[p]->statements[s]->type == NONTERMINAL
                     ? "NON-TERMINAL"
                     : "TERMINAL");
        s++;
      }
      if (p == defs[i]->productionCount - 1)
        printf("  ]\n");
      else
        printf("  ],\n");
      p++;
    }
    printf("]\n");

    i++;
  }
}

void loadFileToBuffer(char *path, char *bp) {
  FILE *fp;
  if ((fp = fopen(path, "r")) == NULL)
    printf("Could not open file\n");
  else {
    while ((*bp++ = fgetc(fp)) != EOF) {
    }
    *--bp = '\0';
  }
  fclose(fp);
}

void printToken(Token tokens[], int it) {
  char *content = tokens[it].content;
  int type = tokens[it].type;
  printf("%d: %s %s\n", it, content, typeToString(type));
}

void trimProdStrings(Token tokens[], int it) {
  tokens[it].content = tokens[it].content + 3;
  char *fp = strstr(tokens[it].content, "</p>");
  tokens[it].content[fp - tokens[it].content] = '\0';
}

char *typeToString(int type) {
  static char *result;
  switch (type) {
  case 0:
    result = "PROD";
    break;
  case 1:
    result = "DEF";
    break;
  default:
    result = "";
  }
  return result;
}

char *concatArrayOfStrings(char **strings, int stringCount, char *separator) {
  char *result = malloc(sizeof(char) * 1000);
  for (int i = 0; i < stringCount; i++) {
    sprintf(result, "%s%s%s", result, separator, strings[i]);
  }

  return result;
}

char *concatStatements(Statement **statements, int statementCount,
                       char *separator) {
  char *result = malloc(sizeof(char) * 1000);
  for (int i = 0; i < statementCount; i++) {
    sprintf(result, "%s%s%s", result, separator, statements[i]->content);
  }

  return result;
}

bool doesSetContainEpsilon(FirstSet *set) {
  for (int i = 0; i < set->itemCount; i++) {
    if (strcmp(set->set[i], "epsilon") == 0) {
      return true;
    }
  }

  return false;
}