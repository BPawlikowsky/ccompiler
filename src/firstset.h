#ifndef FIRSTSETH
#define FIRSTSETH

#include "parserGeneratorUtils.h"

FirstSet *getFirstSet(int defIndex, GeneratorState *genState);
int addFirstSet(char **sourceSet, char **destSet, int itemCount, int destIndex);
bool doesRightHandSideContainTerminal(Definition *definition);
bool doesProductionContainEpsilon(Production *production);
bool doAllProductionsContainEpsilon(Definition *definition);
int returnIndexForFirstNonEpsilonProduction(Definition *definition);
bool doesSetContainEpsilon(FirstSet *set);

#endif // FIRSTSETH