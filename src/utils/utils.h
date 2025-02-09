#pragma once

#include "../types.h"
#include "../tools/logger.h"
#include "../memory.h"

// General
void loadFileToBuffer(char *path, char *bp);
int doesStringExistInArray(char *strings[], int stringsCount,
                           char *itemToCheck);
char *concatArrayOfStrings(char **strings, int stringCount, char *separator);

// Tokens
char *typeToString(int type);
void printToken(Token tokens[], int it);
void trimProdStrings(Token tokens[], int it);

// Definitions and statements
void printDefs(Definition *defs[], int defCount);
char *concatStatements(Statement **statements, int statementCount,
                       char *separator);

// GeneratorState
bool isItemInHistory(GeneratorState *state, int defIndex, SetType setType);
void printHistory(GeneratorState *state, SetType type);
int getDefinitionIndex(GeneratorState *genState, char *statement);
void printDefSymbols(GeneratorState *state);
int getTerminals(char **terminals, GeneratorState *state);
int getNonTerminals(char **nonterminals, GeneratorState *state);
