#ifndef FOLLOWSETH
#define FOLLOWSETH

#include "../types.h"
#include "settools.h"

FollowSet *getFollowSet(int defIndex, GeneratorState *state);
int getStatementIndexFromProduction(Definition *definition,
                                    Production *production);
void saveFollowSet(FollowSet *followSet, GeneratorState *state, int defIndex);
void addFollowSetToSet(FollowSet *followSet, FollowSet *result);
void addFirstSetToSet(FirstSet *firstSet, FollowSet *result);
void addSingleItemToFollowSet(char *content, FollowSet *result);
bool canAllDeriveEpsilon(Production *production, int startIndex,
                         GeneratorState *state);

#endif // !FOLLOWSETH