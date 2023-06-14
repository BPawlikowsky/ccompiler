#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
 * E -> T + E | T
 * T -> int | int * T | (E)
 * Recursive Descent Algorithm
*/
void printTree(Node *tree);
void getLevels(NodeListItem *levels[], Node *tree, int level);
void printLevel(NodeListItem *list);
int counter = 0;

Node parseTree = {.type = "S", .token = NULL, .parent = NULL, .siblings = NULL};
Node *pp = &parseTree;

void testPrint(Token *next, char *func) {
  printf("Function %s, token value: %s\n", func, (*next).content);
}

Token *next;

Node *createNode(char *type, Node *parent, Token *token) {
  Node *node = (Node*)malloc(sizeof(Node));
  node->parent = parent;
  node->type = type;
  node->token = token;
  NodeListItem *nodeItem = (NodeListItem*)malloc(sizeof(NodeListItem));
  nodeItem->node = NULL;
  nodeItem->next = NULL;
  node->siblings = nodeItem;
  return node;
}

void addNode(NodeListItem *siblings, Node *node) {
  NodeListItem *sp = siblings;
  while(sp->next != NULL) {
    sp = sp->next;
  }

  sp->node = node;
  NodeListItem *nextNodeItem = (NodeListItem*)malloc(sizeof(NodeListItem));
  nextNodeItem->node = NULL;
  nextNodeItem->next = NULL;
  sp->next = nextNodeItem;
}

int term(int expectedToken) {
  testPrint(next, "term");
  return (*next++).type == expectedToken;
}

int Ep1() {
  NodeListItem *tempSiblings = (NodeListItem*)malloc(sizeof(NodeListItem));
  tempSiblings->node = NULL;
  tempSiblings->next = NULL;

  Node *anode = createNode("T", pp, NULL);
  addNode(tempSiblings, anode);
  Token *plus = (Token*)malloc(sizeof(Token));
  plus->type = PLUS;
  plus->content = "+";
  Node *bnode = createNode("term", pp, plus);
  addNode(tempSiblings, bnode);
  Node *cnode = createNode("E", pp, NULL);
  addNode(tempSiblings, cnode);
  pp->siblings = tempSiblings;
  return T(pp = anode) && term(PLUS) && E(pp = cnode);
}

int Ep2() {
  NodeListItem *tempSiblings = (NodeListItem*)malloc(sizeof(NodeListItem));
  tempSiblings->node = NULL;
  tempSiblings->next = NULL;

  Node *anode = createNode("T", pp, NULL);
  addNode(tempSiblings, anode);
  pp->siblings = tempSiblings;
  return T(pp = anode);
}

// E -> T + E | T
int E() {
  testPrint(next, "E");
  Token *save = next;
  Node *parent = pp;
  return Ep1() || Ep2((next = save) && (pp = parent));
}

int Tp1() {
  NodeListItem *tempSiblings = (NodeListItem*)malloc(sizeof(NodeListItem));
  tempSiblings->node = NULL;
  tempSiblings->next = NULL;

  Node *anode = createNode("term", pp, next);
  addNode(tempSiblings, anode);
  pp->siblings = tempSiblings;
  return term(NUMBER);
}

int Tp2() {
  NodeListItem *tempSiblings = (NodeListItem*)malloc(sizeof(NodeListItem));
  tempSiblings->node = NULL;
  tempSiblings->next = NULL;

  Node *anode = createNode("term", pp, next);
  addNode(tempSiblings, anode);
  Token *times = (Token*)malloc(sizeof(Token));
  times->type = TIMES;
  times->content = "*";
  Node *bnode = createNode("term", pp, times);
  addNode(tempSiblings, bnode);
  next--;
  Node *cnode = createNode("T", pp, NULL);
  addNode(tempSiblings, cnode);
  pp->siblings = tempSiblings;
  return term(NUMBER) && term(TIMES) && T(pp = cnode);
}

int Tp3() {
  NodeListItem *tempSiblings = (NodeListItem*)malloc(sizeof(NodeListItem));
  tempSiblings->node = NULL;
  tempSiblings->next = NULL;

  Node *anode = createNode("term", pp, next);
  addNode(tempSiblings, anode);
  Node *bnode = createNode("E", pp, NULL);
  addNode(tempSiblings, bnode);
  Token *closedParen = (Token*)malloc(sizeof(Token));
  closedParen->type = CLOSEPAREN;
  closedParen->content = ")";
  Node *cnode = createNode("term", pp, closedParen);
  addNode(tempSiblings, cnode);
  pp->siblings = tempSiblings;
  return term(OPENPAREN) && E(pp = bnode) && term(CLOSEPAREN);
}

// T -> int | int * T | (E)
int T() {
  testPrint(next, "T");
  Token *save = next;
  Node *parent = pp;
  return Tp1() || Tp2((next = save) && (pp = parent)) || Tp3((next = save) && (pp = parent));
}

int main() {
  pp->siblings = (NodeListItem*)malloc(sizeof(NodeListItem));
  pp->siblings->node = NULL;
  pp->siblings->next = NULL;
  Token tokens[100]; 
  char *exp = "(23+2)*8";
  int tokenCount = lexer(exp, tokens, lexicon, tokenTypes, COUNT);
  printf("Parsed tokens:\n");
  for(int i = 0; i < tokenCount; i++) {
    printf("token no%d: %s %s\n", i, tokens[i].content, typeToString(tokens[i].type));
  }

  next = tokens;
  int result = E();
  printf("Parser result: %s\n", result == 0 ? "false" : "true");

  NodeListItem *levels[100];
  for(int i = 0; i < 100; i++){
    levels[i] = malloc(sizeof(NodeListItem));
    levels[i]->next = NULL;
    levels[i]->node = NULL;
  }

  int level = 0;
  getLevels(levels, &parseTree, level);

  int i = 0;
  while(levels[i]->next != NULL) {
    printf("L%d ", i);
    printLevel(levels[i]);
    i++;
  }

  printTree(&parseTree);

  for(int i = 0; i < tokenCount; i++) {
    free(tokens[i].content);
  }
}

void printTree(Node *node){
  printf("Node: %d, type: %s, token: %s, \t\tparent: %d\n",
         node,
         node->type,
         (node->token != NULL) ? node->token->content : "None",
         (node->parent != NULL) ? node->parent : 0
  );

  NodeListItem *sp = node->siblings;

  while(sp->next != NULL && sp->node != NULL) {
    Node *nextNode = sp->node;
    printTree(nextNode);
    sp = sp->next;
  }
}

void addNodeToLevel(NodeListItem *levels[], NodeListItem *siblings, int level) {
  NodeListItem *nextItem = siblings;
  while(nextItem->node != NULL && nextItem->next != NULL) {
    addNode(levels[level], nextItem->node);
    nextItem = nextItem->next;
  }
}

void getLevels(NodeListItem *levels[], Node *tree, int level) {
  printf("Level %d - Start\n", level);
  addNodeToLevel(levels, tree->siblings, level);
  NodeListItem *sp = tree->siblings;
  printf("Level %d - before while\n", level);
  while(sp->next != NULL && sp->node != NULL) {
    Node *nextNode = sp->node;
    int saveLevel = level;
    getLevels(levels, nextNode, ++level);
    level = saveLevel;
    sp = sp->next;
  }
}

void printLevel(NodeListItem *list) {
  NodeListItem *sp = list;
  while(sp->node != NULL) {
    Node *node = sp->node;
    printf("%s\t", (node->type == "term") ? node->token->content : node->type);
    sp = sp->next;
  }
  printf("\n");
}
