#pragma once

#include "../tools/lexer.h"
#include "../tools/firstset.h"
#include "../tools/followset.h"
#include "parsingTable.h"
#include "parserGeneratorUtils.h"
#include "../tools/logger.h"
#include "../memory.h"

#define TOKEN_TYPES_COUNT 5

TokenType tokenTypes[] = {DEF, PROD, COMMENT, WHITESPACE, NEWLINE};

Regex def = {0, .expression = "^[a-zA-Z]+[-_a-zA-Z]*:"};
Regex prod = {0, .expression = "^<p>[[:print:]]*<\\/p>"};
Regex comment = {0, .expression = "^\\/\\/[[:print:]]+"};
Regex whitespace = {0, .expression = "^[ \t]+"};
Regex newline = {0, .expression = "^[\n]"};

Regex *lexicon[] = {&def, &prod, &comment, &whitespace, &newline};

void addStatements(Production *prod, Token tokens[], int it);
void printDefs(Definition *defs[], int defCount);
StatementType getStatementType(char *defs[], char *statement, int defCount);
void addTypeToStatements(Definition *defs[], int defCount, char *strDefs[]);
void saveTerminalsAndNonTerminalsAsText(ParsingTable *table);
void saveParsingTableAsBinaryData(ParsingTable *table);
void saveParsingTableAsCSV(ParsingTable *table);
char *getStringId(int def_index, bool upper_case);
