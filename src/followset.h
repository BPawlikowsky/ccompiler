#ifndef FOLLOWSETH
#define FOLLOWSETH

#include "parserGeneratorUtils.h"

FollowSet *getFollowSet(int defIndex, GeneratorState *state);
int getStatementIndexFromProduction(Definition *definition,
                                    Production *production);
void saveFollowSet(FollowSet *followSet, GeneratorState *state,
                   char *definitionName);
#endif // !FOLLOWSETH