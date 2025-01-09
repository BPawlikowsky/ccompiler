#pragma once

#include "types.h"
#include "logger.h"

extern TokenType tokenTypes[];

int doesStringExistInArray(char *history[], int historyCount,
                           char *itemToCheck);
bool isItemInHistory(GeneratorState *state, int defIndex, SetType setType);
void printHistory(GeneratorState *state, SetType type);
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
void printDefSymbols(GeneratorState *state);
int getTerminals(char **terminals, GeneratorState *state);
int getNonTerminals(char **nonterminals, GeneratorState *state);
int getTerminalIndexFromIndex(ParsingTable *table, char *name);
int getNonTerminalIndexFromIndex(ParsingTable *table, char *name);
char *get_first_set_string(FirstSet *set);
char *get_follow_set_string(FollowSet *set);
