#pragma once

#include "../types.h"
#include "../tools/logger.h"
#include "../utils/utils.h"

extern TokenType tokenTypes[];

int getTerminalIndexFromIndex(ParsingTable *table, char *name);
int getNonTerminalIndexFromIndex(ParsingTable *table, char *name);
