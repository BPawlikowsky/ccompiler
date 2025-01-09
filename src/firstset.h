#ifndef FIRSTSETH
#define FIRSTSETH

#include "parserGeneratorUtils.h"

FirstSet *getFirstSet(int defIndex, GeneratorState *genState);
bool doesRightHandSideContainTerminal(Definition *definition);
bool doesProductionContainEpsilon(Production *production);
bool doAllProductionsContainEpsilon(Definition *definition);
int returnIndexForFirstNonEpsilonProduction(Definition *definition);
bool doesSetContainEpsilon(FirstSet *set);
void addFirstSetToFirstSet(FirstSet *firstSet, FirstSet *result);
void addSingleItemToSet(char *content, FirstSet *result);

#endif // FIRSTSETH