#pragma once
#include "external.h"

// Here you add token type enums for all elements of the compiler
// and than put them in arrays to pass to the lexer
typedef enum TokenType {
  SINGLELINE_COMMENT = 0,
  MULTILINE_COMMENT,
  PREPROCESSOR,
  KEYWORD,
  FLOAT_CONST,
  INTEGER_CONST,
  STRING,
  CHAR,
  IDENTIFIER,
  NEWLINE,
  OPERATOR,
  WHITESPACE,
  COUNT,
  DEF,
  PROD,
  COMMENT,
  INT,
  TIMES,
  PLUS,
  OPENPAR,
  CLOSEPAR,
} TokenType;

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
  char *definition_name;
  char **set;
  int itemCount;
} FirstSet;

typedef struct FollowSet {
  char **set;
  int itemCount;
} FollowSet;

typedef struct FirstSetHistory {
  FirstSet **arr_sets;
  int **arr_visited_count;
} FirstSetHistory;

typedef struct FollowSetHistory {
  FollowSet **arr_sets;
  int **arr_visited_count;
} FollowSetHistory;

typedef struct GeneratorState {
  char **defs;
  Definition **definitions;
  int defCount;
  FirstSetHistory *first_set_history;
  FollowSetHistory *follow_set_history;
  int historyCounter;
  FirstSet **firstSets;
  int firstSetCounter;
  FollowSet **followSets;
  int followSetCounter;
  int terminalCount;
  char **terminals;
  int nonterminalCount;
  char **nonterminals;
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

typedef enum SetType { FIRSTSET, FOLLOWSET } SetType; //
