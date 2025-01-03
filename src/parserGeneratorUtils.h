#pragma once

#include "types.h"

extern TokenType tokenTypes[];

int doesStringExistInArray(char *history[], int historyCount,
                           char *itemToCheck);
bool isItemInHistory(GeneratorState *state, char *defName);
bool isItemInHistoryWithFirstSetCreated(GeneratorState *state, char *defName);
bool isItemInHistoryWithFollowSetCreated(GeneratorState *state, char *defName);
void printHistory(GeneratorState *state);
int getDefinitionIndex(GeneratorState *genState, char *statement);
void printDefs(Definition *defs[], int defCount);
void loadFileToBuffer(char *path, char *bp);
void printToken(Token tokens[], int it);
void trimProdStrings(Token tokens[], int it);
char *typeToString(int type);
char *concatArrayOfStrings(char **strings, int stringCount, char *separator);
char *concatStatements(Statement **statements, int statementCount,
                       char *separator);
bool doesSetContainEpsilon(FirstSet *set);
