#include "firstset.h"

/*
  First set algorythm for definition:
    - if definition's first statement of a production is a terminal,
      add to set.
    - if a definition's production has epsilon on right hand side,
      or all right hand side non-terminals have epsilon in a first set
      of this non terminal, than add epsilon to set.
    - if right hand side production is Def -> A1 A2 A3 B where A1 to A3
      have epsilon in first set than add first set of B to set.
 */
int previous_index = 0;

typedef struct {
  int *arr;
  int counter;
} FirstSetIndexes;

void add_first_set_index(FirstSetIndexes *indexes, int index) {
  for (int i = 0; i < indexes->counter; i++) {
    if (indexes->arr[i] == index) {
      log_debug("Duplicate of index %d", index);
      return;
    }
  }

  log_debug("Index %d added", index);
  indexes->arr[indexes->counter++] = index;
}

FirstSet *getFirstSet(int defIndex, GeneratorState *state) {
  log_debug("FirstSet(%d): Previous index: %d", defIndex, previous_index);
  previous_index = defIndex;

  int *arr = malloc(sizeof(int) * state->defCount);
  FirstSetIndexes indexes = {.counter = 0, .arr = arr};
  FirstSetHistory *first_set_history = state->first_set_history;

  if (*first_set_history->arr_visited_count[defIndex] > 100) {
    log_error("Infinite Loop Error: FirstSet creation detected left "
              "recurrsion, in \'%s\'");
    exit(EXIT_FAILURE);
  }
  first_set_history->arr_visited_count[defIndex]++;

  // HANDLE REPEATING OPERATIONS
  if (isItemInHistory(state, defIndex, FIRSTSET)) {
    return first_set_history->arr_sets[defIndex];
  }

  char **set = malloc(sizeof(char *) * 200);
  FirstSet *savedSet = malloc(sizeof(FirstSet));
  savedSet->itemCount = 0;
  savedSet->set = set;
  Definition *definition = state->definitions[defIndex];

  // ADD CURRENT DEFINITION TO HISTORY
  first_set_history->arr_sets[defIndex] = savedSet;

  // CHECK IF ANY PRODUCTION CONTAINS EPSILON
  if (doAllProductionsContainEpsilon(definition)) {
    if (doesStringExistInArray(set, savedSet->itemCount, "epsilon") == false) {
      addSingleItemToSet("epsilon", savedSet);
    }
  }

  for (int i = 0; i < definition->productionCount; i++) {
    Production *production = definition->productions[i];

    // ADD EPSILON IF ANY PRODUCTION CONTAINS EPSILON
    if (doesProductionContainEpsilon(production)) {
      if (doesStringExistInArray(set, savedSet->itemCount, "epsilon") ==
          false) {
        addSingleItemToSet("epsilon", savedSet);
      }
    }

    for (int i = 0; i < production->statementCount; i++) {
      Statement *statement = production->statements[i];

      // IF TERMINAL, ADD TO SET
      if (statement->type == TERMINAL) {
        if (doesStringExistInArray(set, savedSet->itemCount,
                                   statement->content) == false) {
          addSingleItemToSet(statement->content, savedSet);
        }
        break;
      }

      // IF NONTERMINAL
      if (statement->type == NONTERMINAL) {
        int newDefinitionIndex = getDefinitionIndex(state, statement->content);

        if (newDefinitionIndex < 0) {
          log_error("FirstSet(%d): Could not find non-terminal \'%s\'.",
                    defIndex, statement->content);
        }
        assert(newDefinitionIndex > -1);

        FirstSet *newDefinitionFirstSet;
        if (isItemInHistory(state, newDefinitionIndex, FIRSTSET)) {
          newDefinitionFirstSet =
              first_set_history->arr_sets[newDefinitionIndex];
        } else {
          add_first_set_index(&indexes, newDefinitionIndex);
          // newDefinitionFirstSet = getFirstSet(newDefinitionIndex, state);
          break;
        }

        // IF NONTERMINAL HAS EPSILON IN FIRST SET, GO TO NEXT STATEMENT
        if (newDefinitionFirstSet != NULL &&
            doesSetContainEpsilon(newDefinitionFirstSet)) {
          continue;
        }
        // ELSE ADD STATEMENT'S FIRST SET TO SET AND STOP CHECKING
        else {
          addFirstSetToFirstSet(newDefinitionFirstSet, savedSet);
          break;
        }
      }
    }
  }

  // ADD FISTSET TO SET
  log_debug("First set indexes count: %d", indexes.counter);
  for (int i = 0; i < indexes.counter; i++) {
    saveFirstSet(state, defIndex, savedSet);
    FirstSet *firstSetResult;
    firstSetResult = getFirstSet(indexes.arr[i], state);

    addFirstSetToFirstSet(firstSetResult, savedSet);
  }

  // RETURN SET
  if (savedSet->itemCount == 0) {
    log_error("Error: Firstset empty for definition %s", definition->name);
    exit(EXIT_FAILURE);
  }
  log_trace("FirstSet(%d): added first set for \'%s\'.", defIndex,
            definition->name);
  saveFirstSet(state, defIndex, savedSet);
  state->firstSetCounter++;
  return savedSet;
}

void saveFirstSet(GeneratorState *state, int newDefIndex, FirstSet *set) {
  state->first_set_history->arr_sets[newDefIndex] = set;
  state->firstSets[newDefIndex] = set;
}

bool doesRightHandSideContainTerminal(Definition *definition) {
  for (int i = 0; i < definition->productionCount; i++) {
    Statement *statement = definition->productions[i]->statements[0];
    if (statement->type == TERMINAL) {
      return true;
    }
  }
  return false;
}

bool doesProductionContainEpsilon(Production *production) {
  for (int i = 0; i < production->statementCount; i++) {
    Statement *statement = production->statements[i];
    if (strcmp(statement->content, "epsilon") == 0) {
      return true;
    }
  }

  return false;
}

bool doAllProductionsContainEpsilon(Definition *definition) {
  bool result = false;
  for (int i = 0; i < definition->productionCount; i++) {
    Production *prod = definition->productions[i];
    result = doesProductionContainEpsilon(prod);
  }

  return result;
}

int returnIndexForFirstNonEpsilonProduction(Definition *definition) {
  int result = -1;
  for (int i = 0; i < definition->productionCount; i++) {
    Production *production = definition->productions[i];
    if (doesProductionContainEpsilon(production) == false) {
      result = i;
    }
  }

  return result;
}

void addFirstSetToFirstSet(FirstSet *firstSet, FirstSet *result) {
  for (int j = 0; j < firstSet->itemCount; j++) {
    char *content = firstSet->set[j];
    if (doesStringExistInArray(result->set, result->itemCount, content) ==
        false) {
      addSingleItemToSet(content, result);
    }
  }
}

void addSingleItemToSet(char *content, FirstSet *result) {
  result->set[result->itemCount] = malloc(sizeof(char) * strlen(content) + 1);
  result->set[result->itemCount++] = content;
}