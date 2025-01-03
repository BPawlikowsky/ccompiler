#pragma once

#include "lexer.h"
#include "firstset.h"
#include "followset.h"
#include "parsingTable.h"
#include "parserGeneratorUtils.h"

TokenType tokenTypes[] = {DEF, PROD, WHITESPACE, NEWLINE};

Regex def = {0, .expression = "[a-zA-Z]+[-a-zA-Z]*:"};
Regex prod = {0, .expression = "<p>[[:print:]]*<\\/p>"};
Regex whitespace = {0, .expression = "[ \t]"};
Regex newline = {0, .expression = "[[:space:]]+"};

Regex *lexicon[] = {&def, &prod, &whitespace, &newline};

void addStatements(Production *prod, Token tokens[], int it);
void printDefs(Definition *defs[], int defCount);
StatementType getStatementType(char *defs[], char *statement, int defCount);
void addTypeToStatements(Definition *defs[], int defCount, char *strDefs[]);
void saveTerminalsAndNonTerminalsAsText(ParsingTable *table);
void saveParsingTableAsBinaryData(ParsingTable *table);
void saveParsingTableAsCSV(ParsingTable *table);
