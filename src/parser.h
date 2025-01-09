#pragma once

#include "types.h"
#include "logger.h"
#include "lexer.h"
#include "parserGeneratorUtils.h"

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
                              "\\}|;|:|\\[|\\]|=|,|<|>|\\?|\\/"};
Regex stringRegex = {0, .expression = "\"[[:print:]]*\""};
Regex singleLineCommentRegex = {0, .expression = "\\/\\/[[:print:]]+"};
Regex multiLineCommentRegex = {
    0, .expression = "\\/\\*[[:space:]]*[[:print:]]*[[:space:]]*\\*\\/"};
Regex charRegex = {0, .expression = "\'[[:print:]].*\'"};

// TokenTypes and lexicon need to have enums and Regex
// in the same places to properly identify tokens

#define C_TOKEN_TYPES_COUNT 12

TokenType arr_c_token_types[] = {
    SINGLELINE_COMMENT, MULTILINE_COMMENT, PREPROCESSOR, KEYWORD,
    INTEGER_CONST,FLOAT_CONST,             STRING,       CHAR,
    IDENTIFIER,         NEWLINE,           OPERATOR,     WHITESPACE};

Regex *arr_c_lexicon[] = {&singleLineCommentRegex,
                          &multiLineCommentRegex,
                          &preprocRegex,
                          &keywordRegex,
                          &integerRegex,
                          &floatRegex,
                          &stringRegex,
                          &charRegex,
                          &identifierRegex,
                          &newlineRegex,
                          &operatorRegex,
                          &whitepsaceRegex};

// helper array to print token types
char *arr_c_token_type_strings[12] = {
    "SINGLELINE_COMMENT", "MULTILINE_COMMENT", "PREPROCESSOR", "KEYWORD",
    "INTEGER_CONST","FLOAT_CONST",             "STRING",       "CHAR",
    "IDENTIFIER",         "NEWLINE",           "OPERATOR",     "WHITESPACE"};

// SIMPLE GRAMMER STUFF

Regex intReg = {0, .expression = "int"};
Regex timesReg = {0, .expression = "\\*"};
Regex plusReg = {0, .expression = "\\+"};
Regex openReg = {0, .expression = "\\("};
Regex closeReg = {0, .expression = "\\)"};

#define SG_TOKEN_TYPES_COUNT 6

TokenType arr_sg_token_types[] = {INT,     TIMES,    PLUS,
                                  OPENPAR, CLOSEPAR, WHITESPACE};

Regex *arr_sg_lexicon[] = {&openReg,  &closeReg, &plusReg,
                           &timesReg, &intReg,   &whitepsaceRegex};

char *arr_sg_token_type_strings[6] = {"INT",  "TIMES", "PLUS",
                                      "OPEN", "CLOSE", "WHITE"};

int getItemTokenType(char *item, ParsingTable *table);