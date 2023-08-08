#ifndef TOKEN
#define TOKEN typedef enum TokenType {\
  PROD = 0,\
  DEF,\
  COUNT \
} TokenType;
#endif
#include "lexer.h"

TokenType tokenTypes[] = {
  PROD,
  DEF
};


Regex def    = { 0, .expression = "[a-zA-Z][a-zA-Z-]+:" };
Regex prod   = { 0, .expression = "<p>.*<\\/p>" };

Regex *lexicon[] = {
  &prod,
  &def
};

char *typeToString(int type) {
  static char *result;
  switch (type) {
    case 0: result = "PROD";break;  
    case 1: result = "DEF";break;
    default: result = "";
  }
}

