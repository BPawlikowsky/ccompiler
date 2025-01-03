#pragma once
#include "external.h"

// typedef enum TokenType {
//   SINGLELINE_COMMENT = 0,
//   MULTILINE_COMMENT,
//   PREPROCESSOR,
//   KEYWORD,
//   FLOAT_CONST,
//   INTEGER_CONST,
//   STRING,
//   CHAR,
//   IDENTIFIER,
//   NEWLINE,
//   OPERATOR,
//   WHITESPACE,
//   COUNT
// } TokenType;

typedef enum TokenType { DEF = 0, PROD, WHITESPACE, NEWLINE, COUNT } TokenType;

// typedef enum TokenType {
//   INT,
//   TIMES,
//   PLUS,
//   OPENPAR,
//   CLOSEPAR,
//   WHITESPACE,
//   COUNT
// } TokenType;

typedef struct Token {
  TokenType type;
  char *content;
} Token;

struct NodeListItem;

typedef struct Regex {
  regex_t r;
  char *expression;
} Regex;

typedef struct Node {
  char *type;
  Token *token;
  struct Node *parent;
  struct NodeListItem *siblings;
} Node;

typedef struct NodeListItem {
  Node *node;
  struct NodeListItem *next;
} NodeListItem;

typedef enum StatementType { TERMINAL = 0, NONTERMINAL } StatementType;

typedef struct Statement {
  StatementType type;
  int stringLength;
  char *content;
} Statement;

typedef struct Production {
  int statementCount;
  Statement **statements;
} Production;

typedef struct Definition {
  char *name;
  int productionCount;
  Production **productions;
} Definition;

typedef struct FirstSet {
  char **set;
  int itemCount;
} FirstSet;

typedef struct FollowSet {
  char **set;
  int itemCount;
} FollowSet;

typedef struct History {
  char *item;
  bool isFirstSetCreated;
  bool isFollowSetCreated;
} History;

typedef struct GeneratorState {
  char **defs;
  Definition **definitions;
  int defCount;
  History **history;
  int historyCounter;
  FirstSet **firstSets;
  int firstSetCounter;
  FollowSet **followSets;
  int followSetCounter;
} GeneratorState;

typedef struct TableEntry {
  int row;
  int col;
  Production *production;
  bool error;
} TableEntry;

typedef struct ParsingTable {
  int terminalCount;
  int nonterminalCount;
  int tableSize;
  char **terminals;
  char **nonterminals;
  TableEntry **table;
} ParsingTable;
