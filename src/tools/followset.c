#include "followset.h"

/*
  Follow set algoryhm for given definition:
  - given next symbol b following definition in right hand side production of A
    A -> aXb where X is the definition, if b is a terminal, add to set.
  - if b is a non-terminal and doesn't contain epsilon,
    than first of b is in the follow set of X, so add first set of b to set.
  - if b is a non-terminal and is the last statement in a production,
    and the production contains an epsilon in it's first set,
    add follow set of A to set.
*/

int previousIndex = 0;

typedef struct {
  int *arr;
  int counter;
} FollowSetIndexes;

void add_follow_set_index(FollowSetIndexes *indexes, int index) {
  for (int i = 0; i < indexes->counter; i++) {
    if (indexes->arr[i] == index) {
      log_debug("Duplicate of index %d", index);
      return;
    }
  }

  log_debug("Index %d added", index);
  indexes->arr[indexes->counter++] = index;
}

FollowSet *getFollowSet(int defIndex, GeneratorState *state) {
  log_trace("FollowSet(%d): Previous index: %d", defIndex, previousIndex);
  previousIndex = defIndex;

  int arr[100];

  FollowSetIndexes follow_set_indexes = {.arr = arr, .counter = 0};

  FollowSetHistory *follow_set_history = state->follow_set_history;

  // HANDLE REPEATING OPERATIONS
  if (isItemInHistory(state, defIndex, FOLLOWSET)) {
    return follow_set_history->arr_sets[defIndex];
  }

  FollowSet *result = get_memory(sizeof(FollowSet));
  char **set = get_memory(sizeof(char *) * 200);
  result->itemCount = 0;
  result->set = set;

  Definition *definition = state->definitions[defIndex];

  for (int i = 0; i < state->defCount; i++) {
    if (i == defIndex) {
      continue;
    }

    for (int p = 0; p < state->definitions[i]->productionCount; p++) {
      Production *production = state->definitions[i]->productions[p];
      // GET INDEX OF CURRENT DEFINITION IN PRODUCTION
      int statementIndex = getStatementIndexFromProduction(
          definition, state->definitions[i]->productions[p]);

      // IF DEFINITION IS NOT IN PRODUCION, OR IS THE SAME AS INVOKED
      // DEFINITION, SKIP
      if (statementIndex < 0) {
        continue;
      }

      for (int s = statementIndex; s < production->statementCount; s++) {
        Statement *nextStatement = production->statements[s + 1];

        if (s + 1 == production->statementCount) {
          break;
        }

        // IF NEXT STATEMENT IS A TERMINAL,
        // ADD TO SET, AND GO TO NEXT PRODUCTION
        if (nextStatement->type == TERMINAL) {
          char *content = nextStatement->content;
          if (doesStringExistInArray(set, result->itemCount, content) ==
                  false &&
              strcmp(content, "epsilon") != 0) {
            addSingleItemToFollowSet(content, result);
          }
          continue;
        }

        if (nextStatement->type == NONTERMINAL) {
          int newDefIndex = getDefinitionIndex(state, nextStatement->content);

          if (newDefIndex < 0) {
            log_error("FollowSet(%d): could not find non-terminal \'%s\'.",
                      defIndex, nextStatement->content);
          }
          assert(newDefIndex > -1);

          FirstSet *firstSet = state->firstSets[newDefIndex];

          // ADD FIRST SET
          addFirstSetToSet(firstSet, result);

          bool hasEpsilon = doesSetContainEpsilon(firstSet);

          if (!hasEpsilon) {
            continue;
          }
        }
      }

      if (statementIndex == production->statementCount - 1 ||
          canAllDeriveEpsilon(production, statementIndex + 1, state)) {
        add_follow_set_index(&follow_set_indexes, i);
      }
    }
  }

  // ADD CURRENT DEFINITION TO HISTORY
  follow_set_history->arr_sets[defIndex] = result;

  log_debug("Follow set indexes count: %d", follow_set_indexes.counter);
  for (int i = 0; i < follow_set_indexes.counter; i++) {
    saveFollowSet(result, state, defIndex);
    FollowSet *followSetResult;
    followSetResult = getFollowSet(follow_set_indexes.arr[i], state);

    addFollowSetToSet(followSetResult, result);
  }

  if (result->itemCount == 0) {
    log_error("Error: FollowSet(%d) \'%s\' is empty.", defIndex,
              definition->name);
    assert(result->itemCount > 0);
  }

  log_trace("Added Follow set(%d) created for %s", defIndex, definition->name);

  saveFollowSet(result, state, defIndex);
  state->followSetCounter++;
  return result;
}

void saveFollowSet(FollowSet *followSet, GeneratorState *state, int defIndex) {
  state->follow_set_history->arr_sets[defIndex] = followSet;
  state->followSets[defIndex] = followSet;
}

int getStatementIndexFromProduction(Definition *definition,
                                    Production *production) {
  for (int j = 0; j < production->statementCount; j++) {
    if (production->statements[j]->type == NONTERMINAL &&
        strcmp(production->statements[j]->content, definition->name) == 0) {
      return j;
    }
  }

  return -1;
}

void addFollowSetToSet(FollowSet *followSet, FollowSet *result) {
  for (int j = 0; j < followSet->itemCount; j++) {
    char *content = followSet->set[j];
    if (doesStringExistInArray(result->set, result->itemCount, content) ==
        false) {
      addSingleItemToFollowSet(content, result);
    }
  }
}

void addFirstSetToSet(FirstSet *firstSet, FollowSet *result) {
  for (int j = 0; j < firstSet->itemCount; j++) {
    char *content = firstSet->set[j];
    if (doesStringExistInArray(result->set, result->itemCount, content) ==
            false &&
        strcmp(content, "epsilon")) {
      addSingleItemToFollowSet(content, result);
    }
  }
}

void addSingleItemToFollowSet(char *content, FollowSet *result) {
  result->set[result->itemCount] = get_memory(sizeof(char) * strlen(content) + 1);
  result->set[result->itemCount++] = content;
}

// New helper function to check if all symbols in a sequence can derive epsilon
bool canAllDeriveEpsilon(Production *production, int startIndex,
                         GeneratorState *state) {
  for (int i = startIndex; i < production->statementCount; i++) {
    Statement *stmt = production->statements[i];

    if (stmt->type == TERMINAL) {
      if (strcmp(stmt->content, "epsilon") != 0) {
        return false;
      }
    } else {
      int defIndex = getDefinitionIndex(state, stmt->content);
      if (defIndex < 0 || !doesSetContainEpsilon(state->firstSets[defIndex])) {
        return false;
      }
    }
  }
  return true;
}
