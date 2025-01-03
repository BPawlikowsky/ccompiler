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

FollowSet *getFollowSet(int defIndex, GeneratorState *state) {
  FollowSet *result = malloc(sizeof(FollowSet));
  char **set = malloc(sizeof(char *) * 100);
  result->itemCount = 0;
  result->set = set;

  Definition *definition = state->definitions[defIndex];

  // printf("\nEntering getFollowSet(%s)\n", definition->name);

  if (isItemInHistory(state, definition->name)) {
    if (isItemInHistoryWithFollowSetCreated(state, definition->name)) {
      int foundDefIndex = getDefinitionIndex(state, definition->name);
      assert(foundDefIndex > -1);

      if (foundDefIndex < 0) {
        printf("index less than 0, foundDefIndex\n");
      }

      return state->followSets[foundDefIndex];
    }
  }

  // ADD CURRENT DEFINITION TO HISTORY
  History *item = malloc(sizeof(History));
  item->item = definition->name;
  item->isFollowSetCreated = false;
  state->history[state->historyCounter++] = item;

  for (int i = 0; i < state->defCount; i++) {
    for (int p = 0; p < state->definitions[i]->productionCount; p++) {
      // GET INDEX OF CURRENT DEFINITION IN PRODUCTION
      int statementIndex = getStatementIndexFromProduction(
          definition, state->definitions[i]->productions[p]);

      // IF DEFINITION IS NOT IN PRODUCION, SKIP
      if (statementIndex < 0) {
        continue;
      }

      Production *production = state->definitions[i]->productions[p];

      for (int s = statementIndex; s < production->statementCount; s++) {
        Statement *nextStatement = production->statements[s + 1];

        // IF ONLY ONE STATEMENT IN PRODUCTION,
        // ADD FOLLOW OF DEFINITION
        if (s + 1 == production->statementCount) {
          saveFollowSet(result, state, definition->name);
          FollowSet *followSetResult = getFollowSet(i, state);

          for (int j = 0; j < followSetResult->itemCount; j++) {
            if (doesStringExistInArray(set, result->itemCount,
                                       followSetResult->set[j]) == false &&
                strcmp(followSetResult->set[j], "epsilon") != 0) {
              set[result->itemCount++] = followSetResult->set[j];
            }
          }

          // printf("Added follow set of %s\n", state->definitions[i]->name);
          continue;
        }

        // IF NEXT STATEMENT IS A TERMINAL,
        // ADD TO SET, AND GO TO NEXT PRODUCTION
        if (nextStatement->type == TERMINAL) {
          if (doesStringExistInArray(set, result->itemCount,
                                     nextStatement->content) == false &&
              strcmp(nextStatement->content, "epsilon") != 0) {
            set[result->itemCount++] = nextStatement->content;
            // printf("\nAdded terminal \"%s\"\n", nextStatement->content);
          }
          break;
        }

        if (nextStatement->type == NONTERMINAL) {
          int newDefIndex = getDefinitionIndex(state, nextStatement->content);

          assert(newDefIndex > -1);
          if (newDefIndex < 0) {
            printf("index less than 0, newDefIndex\n");
          }

          FirstSet *firstSet = state->firstSets[newDefIndex];

          for (int j = 0; j < firstSet->itemCount; j++) {
            if (doesStringExistInArray(set, result->itemCount,
                                       firstSet->set[j]) == false &&
                strcmp(firstSet->set[j], "epsilon") != 0) {
              set[result->itemCount++] = firstSet->set[j];
            }
          }

          // printf("\nAdded first set of %s\n",
          //  state->definitions[newDefIndex]->name);

          // IF NONTERMINAL'S FIRST SET DOES CONTAIN EPSILON,
          // SKIP
          if (doesSetContainEpsilon(firstSet) &&
              s + 1 < production->statementCount - 1) {
            // printf("\nFirst set of %s contains epsilon, skipping
            // statement\n",
            //        state->definitions[newDefIndex]->name);
            continue;
          }

          // IF NEXT STATEMENT IS THE LAST STATEMENT ADD FOLLOW SET OF
          // I-DEFINITION TO SET
          if (doesSetContainEpsilon(firstSet) == false &&
              s + 1 == production->statementCount - 1) {
            saveFollowSet(result, state, definition->name);
            FollowSet *followSetResult = getFollowSet(i, state);

            for (int j = 0; j < followSetResult->itemCount; j++) {
              if (doesStringExistInArray(set, result->itemCount,
                                         followSetResult->set[j]) == false) {
                set[result->itemCount++] = followSetResult->set[j];
              }
            }

            // printf("Added follow set of %s\n", state->definitions[i]->name);
          }
        }
      }
    }
  }

  saveFollowSet(result, state, definition->name);
  state->followSetCounter++;
  return result;
}

void saveFollowSet(FollowSet *followSet, GeneratorState *state,
                   char *definitionName) {
  int newDefIndex = getDefinitionIndex(state, definitionName);

  if (newDefIndex < 0) {
    printf("index less than 0, newDefIndex\n");
  }

  state->followSets[newDefIndex] = followSet;

  for (int i = 0; i < state->historyCounter; i++) {
    if (strcmp(definitionName, state->history[i]->item) == 0) {
      state->history[i]->isFollowSetCreated = true;
    }
  }
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
