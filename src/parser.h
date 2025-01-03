#pragma once

#include "types.h"
#include "lexer.h"
#include "parserGeneratorUtils.h"

TokenType tokenTypes[] = {
    SINGLELINE_COMMENT, MULTILINE_COMMENT, PREPROCESSOR, KEYWORD,
    FLOAT_CONST,        INTEGER_CONST,     STRING,       CHAR,
    IDENTIFIER,         NEWLINE,           OPERATOR,     WHITESPACE};

Regex preprocRegex = {0, .expression = "#[[:print:]]+"};
Regex identifierRegex = {0, .expression = "[_]?[[:alpha:]][[:alnum:]_]*"};
Regex integerRegex = {0, .expression = "[[:digit:]]+"};
Regex floatRegex = {0, .expression =
                           "[[:digit:]]+.[[:digit:]]+f|[[:digit:]]+.[["
                           ":digit:]]+d|[[:digit:]]+.[[:digit:]]+"};
Regex keywordRegex = {
    0, .expression =
           "auto|break|case|char|const|continue|default|do|double|else|"
           "enum|extern|float|for|goto|if|inline|int|long|register|"
           "restrict|return|short|signed|sizeof|static|struct|switch|"
           "typedef|union|unsigned|void|volatile|while"};
Regex whitepsaceRegex = {0, .expression = "[ \t]"};
Regex newlineRegex = {0, .expression = "\n"};
Regex operatorRegex = {0, .expression =
                              "\\+\\+|--|\\+=|-=|<=|>=|\\!=|==|<<|>>|&&|"
                              "\\|\\||\\*|&|\\+|-|~|\\!|\%|\\^|\\||\\(|\\)|\\{|"
                              "\\}|;|\\[|\\]|=|,|<|>"};
Regex stringRegex = {0, .expression = "\"[[:print:]]*\""};
Regex singleLineCommentRegex = {0, .expression = "\\/\\/[[:print:]]+"};
Regex multiLineCommentRegex = {
    0, .expression = "\\/\\*[[:space:]]*[[:print:]]*[[:space:]]*\\*\\/"};
Regex charRegex = {0, .expression = "\'[[:print:]].*\'"};

Regex *lexicon[] = {&singleLineCommentRegex,
                    &multiLineCommentRegex,
                    &preprocRegex,
                    &keywordRegex,
                    &floatRegex,
                    &integerRegex,
                    &stringRegex,
                    &charRegex,
                    &identifierRegex,
                    &newlineRegex,
                    &operatorRegex,
                    &whitepsaceRegex};
char *tokenTypeStrings[12] = {
    "SINGLELINE_COMMENT", "MULTILINE_COMMENT", "PREPROCESSOR", "KEYWORD",
    "FLOAT_CONST",        "INTEGER_CONST",     "STRING",       "CHAR",
    "IDENTIFIER",         "NEWLINE",           "OPERATOR",     "WHITESPACE"};

// SIMPLE GRAMMER STUFF

// TokenType tokenTypes[] = {
// SINGLELINE_COMMENT, MULTILINE_COMMENT, PREPROCESSOR, KEYWORD,
// INT, TIMES, PLUS, OPENPAR, CLOSEPAR, WHITESPACE};

// Regex intReg = {0, .expression = "int"};
// Regex timesReg = {0, .expression = "\\*"};
// Regex plusReg = {0, .expression = "\\+"};
// Regex openReg = {0, .expression = "\\("};
// Regex closeReg = {0, .expression = "\\)"};

// Regex *lexicon[] = {
//     &openReg, &closeReg, &plusReg, &timesReg, &intReg, &whitepsaceRegex};

// char *tokenTypeStrings[12] = {
//     "INT", "TIMES", "PLUS", "OPEN", "CLOSE", "WHITE"};

int getItemTokenType(char *item, ParsingTable *table);