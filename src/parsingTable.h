#pragma once

#include "parserGeneratorUtils.h"

ParsingTable *createParsingTable(GeneratorState *state);
bool isStringInFollowSet(char *str, FollowSet *set);
bool addItemToTable(int row, int col, Production *production,
                    ParsingTable *table);
