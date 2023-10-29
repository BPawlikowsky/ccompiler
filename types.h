#include <../../include/regex.h>
#ifndef TOKEN
#define TOKEN typedef enum TokenType {\
  DEF = 0,\
  PROD,\
  COUNT \
} TokenType;
#endif

TOKEN
struct NodeListItem;

typedef struct Regex{
  regex_t r;
  char *expression;
} Regex;

typedef struct Token{
  TokenType type;
  char *content;
} Token;

typedef struct Node {
  char *type;
  Token *token;
  struct Node *parent;
  struct NodeListItem *siblings;
} Node;

typedef struct NodeListItem{
  Node *node;
  struct NodeListItem *next;
} NodeListItem;

typedef enum StatementType {
  TERMINAL = 0,
  NONTERMINAL
} StatementType;

typedef struct Statement {
  StatementType type;
} Statement;

typedef struct Production {
} Production;

typedef struct Definition {
} Definition;

