#ifndef TOKEN
#define TOKEN typedef enum TokenType {\
  NUMBER = 0,\
  OPENPAREN,\
  CLOSEPAREN,\
  PLUS,\
  TIMES,\
  EPSILON,\
  COUNT\
} TokenType;
#endif
#include "lexer.h"

Regex number    = { 0, .expression = "[[:digit:]]+" };
Regex openparen    = { 0, .expression = "[(]" };
Regex closeparen    = { 0, .expression = "[)]" };
Regex plus    = { 0, .expression = "[+]" };
Regex times    = { 0, .expression = "[*]" };
Regex eps    = { 0, .expression = "[$]" };

TokenType tokenTypes[] = {
  NUMBER,
  OPENPAREN,
  CLOSEPAREN,
  PLUS,
  TIMES,
  EPSILON
};

Regex *lexicon[] = {
  &number,
  &openparen,
  &closeparen,
  &plus,
  &times,
  &eps
};

char *typeToString(int type) {
  switch (type) {
    case 0: return "NUMBER";  
    case 1: return "OPENPAREN";  
    case 2: return "CLOSEPAREN";  
    case 3: return "PLUS";  
    case 4: return "TIMES";  
    case 5: return "EPSILON";  
    default: return "";
  }
}

int E();
int T();
int Y();
int X();
