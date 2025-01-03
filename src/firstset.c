#include "firstset.h"

int addFirstSet(char **sourceSet, char **destSet, int itemCount,
                int destIndex) {
  int counter = 0;
  int itemsAdded = 0;
  while (counter < itemCount) {
    if (doesStringExistInArray(destSet, destIndex, sourceSet[counter]) ==
        false) {
      destSet[destIndex + counter] = sourceSet[counter];
      itemsAdded++;
    }
    counter++;
  };
  return itemsAdded;
}

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
FirstSet *getFirstSet(int defIndex, GeneratorState *genState) {
  int setCounter = 0;
  char **set = malloc(sizeof(char *) * 1000);
  Definition *definition = genState->definitions[defIndex];

  // HANDLE REPEATING OPERATIONS
  if (isItemInHistory(genState, definition->name) &&
      isItemInHistoryWithFirstSetCreated(genState, definition->name)) {
    int foundDefIndex = getDefinitionIndex(genState, definition->name);
    assert(foundDefIndex > -1);
    // printf("error: index = -1 for string \"%s\"\n", definition->name);

    FirstSet *result = genState->firstSets[foundDefIndex];
    return result;
  }

  // ADD CURRENT DEFINITION TO HISTORY
  History *item = malloc(sizeof(History));
  item->item = definition->name;
  item->isFirstSetCreated = false;
  genState->history[genState->historyCounter++] = item;

  // CHECK IF ANY PRODUCTION CONTAINS EPSILON
  if (doAllProductionsContainEpsilon(definition)) {
    if (doesStringExistInArray(set, setCounter, "epsilon") == false) {
      set[setCounter++] = "epsilon";
    }
  }

  for (int i = 0; i < definition->productionCount; i++) {
    Production *production = definition->productions[i];

    // ADD EPSILON IF ANY PRODUCTION CONTAINS EPSILON
    if (doesProductionContainEpsilon(production)) {
      if (doesStringExistInArray(set, setCounter, "epsilon") == false) {
        set[setCounter++] = "epsilon";
      }
    }

    for (int i = 0; i < production->statementCount; i++) {
      Statement *statement = production->statements[i];

      // IF TERMINAL, ADD TO SET
      if (statement->type == TERMINAL) {
        if (doesStringExistInArray(set, setCounter, statement->content) ==
            false) {
          set[setCounter++] = statement->content;
        }
        break;
      }

      // IF NONTERMINAL
      if (statement->type == NONTERMINAL) {
        int newDefinitionIndex =
            getDefinitionIndex(genState, statement->content);
        assert(newDefinitionIndex > -1);
        // printf("error: index = -1 for string \"%s\"\n", statement->content);

        FirstSet *newDefinitionFirstSet =
            getFirstSet(newDefinitionIndex, genState);

        // IF NONTERMINAL HAS EPSILON IN FIRST SET, GO TO NEXT STATEMENT
        if (doesSetContainEpsilon(newDefinitionFirstSet)) {
          continue;
        }
        // ELSE ADD STATEMENT'S FIRST SET TO SET AND STOP CHECKING
        else {
          for (int j = 0; j < newDefinitionFirstSet->itemCount; j++) {
            if (doesStringExistInArray(
                    set, setCounter, newDefinitionFirstSet->set[j]) == false) {
              set[setCounter++] = newDefinitionFirstSet->set[j];
            }
          }
          break;
        }
      }
    }
  }

  // RETURN SET
  FirstSet *savedSet = malloc(sizeof(FirstSet));
  savedSet->itemCount = setCounter;
  savedSet->set = set;
  int newDefIndex = getDefinitionIndex(genState, definition->name);
  // printf("defIndex: %d, defName: %s\n", newDefIndex, definition->name);
  genState->firstSets[newDefIndex] = savedSet;
  genState->firstSetCounter++;
  for (int i = 0; i < genState->historyCounter; i++) {
    if (strcmp(definition->name, genState->history[i]->item) == 0) {
      genState->history[i]->isFirstSetCreated = true;
    }
  }
  return savedSet;
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

int func(int (*funcPtr)(int, int)) { return funcPtr(1, 2); }