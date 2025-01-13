#ifndef FIRSTSETH
#define FIRSTSETH

#include "../types.h"
#include "settools.h"

FirstSet *getFirstSet(int defIndex, GeneratorState *genState);
bool doesRightHandSideContainTerminal(Definition *definition);
bool doesProductionContainEpsilon(Production *production);
bool doAllProductionsContainEpsilon(Definition *definition);
int returnIndexForFirstNonEpsilonProduction(Definition *definition);
bool doesSetContainEpsilon(FirstSet *set);
void addFirstSetToFirstSet(FirstSet *firstSet, FirstSet *result);
void addSingleItemToSet(char *content, FirstSet *result);
void saveFirstSet(GeneratorState *state, int newDefIndex, FirstSet *set);

#endif // FIRSTSETH