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

bool isItemInHistory(GeneratorState *state, int defIndex, SetType setType) {
  if (setType == FIRSTSET) {
    if (state->first_set_history->arr_sets[defIndex] != NULL) {
      return true;
    }
  } else {
    if (state->follow_set_history->arr_sets[defIndex] != NULL) {
      return true;
    }
  }
  return false;
}

void printHistory(GeneratorState *state, SetType type) {
  if (type == FIRSTSET) {
    for (int i = 0; i < state->defCount; i++) {
      log_trace("%d %s", i, state->first_set_history->arr_sets[i]);
    }
  } else {
    for (int i = 0; i < state->defCount; i++) {
      log_trace("%d %s", i, state->follow_set_history->arr_sets[i]);
    }
  }
}

char *getStringId(int def_index, bool upper_case) {
  // if index between 0 - 25 -> add character
  // if index higher than 25 -> substract 25 and bump the counter
  int multiplier = 0;
  int i_result = def_index;

  int alphabet_size = 24;
  int alphabet_offset = (upper_case == true ? 65 : 97);

  if (def_index >= alphabet_size) {
    multiplier = def_index != 0 ? (def_index / alphabet_size) : 0;
    i_result = (def_index - (alphabet_size * multiplier));
  }
  i_result += alphabet_offset;

  char *s_result = malloc(sizeof(char) * 3);
  if (multiplier == 0)
    sprintf(s_result, "%c", i_result);
  else
    sprintf(s_result, "%c%c", i_result, multiplier + alphabet_offset - 1);

  return s_result;
}

void printDefs(Definition *defs[], int defCount) {
  int i = 0;
  char *str = malloc(sizeof(char) * 200);
  while (i < defCount) {
    Definition *def = defs[i];
    sprintf(str, "%d: \"%s\": ", i, def->name);
    int p = 0;
    while (p < def->productionCount) {
      int s = 0;
      Production *prod = def->productions[p];
      while (s < prod->statementCount) {
        Statement *statement = prod->statements[s];
        if (s == prod->statementCount - 1)
          sprintf(str, "%s\"%s\"(%s)", str, statement->content,
                  statement->type == NONTERMINAL ? "N" : "T");
        else
          sprintf(str, "%s\"%s\"(%s) ", str, statement->content,
                  statement->type == NONTERMINAL ? "N" : "T");
        s++;
      }
      if (p == def->productionCount - 1) {
        sprintf(str, "%s", str);
      } else {
        sprintf(str, "%s | ", str);
      }
      p++;
    }
    log_trace("%s", str);

    i++;
  }
}

int getTerminalIndex(char **terminals, int count, char *terminal) {
  for (int i = 0; i < count; i++) {
    if (strcmp(terminals[i], terminal) == 0) {
      return i;
    }
  }

  return -1;
}

int getDefinitionIndexFromDefs(Definition **defs, int count, char *name) {
  for (int i = 0; i < count; i++) {
    if (strcmp(defs[i]->name, name) == 0) {
      return i;
    }
  }

  return -1;
}

void printDefSymbols(GeneratorState *state) {
  Definition **defs = state->definitions;
  char **terminals = state->terminals;
  int terminalCount = state->terminalCount;
  int defCount = state->defCount;
  int i = 0;
  char *str = malloc(sizeof(char) * 200);

  while (i < defCount) {
    Definition *def = defs[i];
    sprintf(str, "%d: %s -> ", i, getStringId(i, true));
    int p = 0;
    while (p < def->productionCount) {
      int s = 0;
      Production *prod = def->productions[p];
      while (s < prod->statementCount) {
        Statement *statement = prod->statements[s];
        int definitionIndex =
            getDefinitionIndexFromDefs(defs, defCount, statement->content);
        int terminalIndex =
            getTerminalIndex(terminals, terminalCount, statement->content);

        bool is_epsilon = strcmp(statement->content, "epsilon") == 0;

        char *output = statement->type == NONTERMINAL
                           ? getStringId(definitionIndex, true)
                           : getStringId(terminalIndex, false);
        output = is_epsilon ? "eps" : output;
        if (s == prod->statementCount - 1)
          sprintf(str, "%s%s", str, output);
        else
          sprintf(str, "%s%s ", str, output);
        s++;
      }
      if (p == def->productionCount - 1) {
        sprintf(str, "%s", str);
      } else {
        sprintf(str, "%s | ", str);
      }
      p++;
    }
    log_trace("%s", str);

    i++;
  }
}

void loadFileToBuffer(char *path, char *bp) {
  FILE *fp;
  if ((fp = fopen(path, "r")) == NULL)
    log_error("Could not open file\n");
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
  terminals[counter++] = "$";

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

char *get_first_set_string(FirstSet *set) {
  char *result = malloc(sizeof(char) * set->itemCount * 50);
  for (int i = 0; i < set->itemCount; i++) {
    if (i < set->itemCount - 1)
      sprintf(result, "%s \'%s\' |", result, set->set[i]);
    else
      sprintf(result, "%s \'%s\'", result, set->set[i]);
  }

  return result;
}

char *get_follow_set_string(FollowSet *set) {
  char *result = malloc(sizeof(char) * set->itemCount * 50);
  for (int i = 0; i < set->itemCount; i++) {
    if (i < set->itemCount - 1)
      sprintf(result, "%s \'%s\' |", result, set->set[i]);
    else
      sprintf(result, "%s \'%s\'", result, set->set[i]);
  }

  return result;
}
