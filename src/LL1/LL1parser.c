#include "LL1parser.h"
#include "parserGeneratorUtils.h"

ParsingTable *loadParsingTable() {
  FILE *fp = fopen("ptable.dat", "rb");
  if (fp == NULL) {
    log_error("error: could not open file");
  }

  ParsingTable *ptp = get_memory(sizeof(ParsingTable));

  fread(ptp, sizeof(ParsingTable), 1, fp);
  log_trace("table size: %d", ptp->tableSize);
  log_trace("nonterminal count: %d", ptp->nonterminalCount);
  log_trace("terminal count: %d", ptp->terminalCount);

  TableEntry **table = get_memory(sizeof(TableEntry) * ptp->tableSize);

  for (int tableEntryIndex = 0; tableEntryIndex < ptp->tableSize;
       tableEntryIndex++) {
    table[tableEntryIndex] = get_memory(sizeof(TableEntry));
    fread(table[tableEntryIndex], sizeof(TableEntry), 1, fp);

    if (table[tableEntryIndex]->error == false) {
      table[tableEntryIndex]->production = get_memory(sizeof(Production));
      Production *production = table[tableEntryIndex]->production;
      fread(production, sizeof(Production), 1, fp);

      production->statements =
          get_memory(sizeof(Statement) * production->statementCount);

      for (int statementIndex = 0; statementIndex < production->statementCount;
           statementIndex++) {
        production->statements[statementIndex] = get_memory(sizeof(Statement));
        Statement *statement = production->statements[statementIndex];
        fread(statement, sizeof(Statement), 1, fp);
        statement->content = get_memory(sizeof(char) * statement->stringLength);
        fread(statement->content, sizeof(char) * statement->stringLength, 1,
              fp);
      }
    }
  }
  fclose(fp);

  ptp->table = table;

  FILE *file = fopen("nonterminals.txt", "r");
  ptp->nonterminals = get_memory(sizeof(char *) * ptp->nonterminalCount);
  for (int i = 0; i < ptp->nonterminalCount; i++) {
    char *result = get_memory(sizeof(char) * 50);
    fscanf(file, "%s\n", result);
    ptp->nonterminals[i] = result;
  }
  fclose(file);

  file = fopen("terminals.txt", "r");
  ptp->terminals = get_memory(sizeof(char *) * ptp->terminalCount);
  for (int i = 0; i < ptp->terminalCount; i++) {
    char *result = get_memory(sizeof(char) * 50);
    fscanf(file, "%s\n", result);
    ptp->terminals[i] = result;
  }
  fclose(file);

  // check
  for (int i = 0; i < ptp->nonterminalCount * ptp->terminalCount; i++) {
    if (table[i]->error == false) {
      Production *prod = table[i]->production;

      log_trace("%d: Statement count: %d", i, prod->statementCount);

      for (int j = 0; j < prod->statementCount; j++) {
        log_trace("\"%s\" ", prod->statements[j]->content);
      }
    }
  }

  log_trace("Non-terminals");
  for (int i = 0; i < ptp->nonterminalCount; i++) {
    log_trace("%s", ptp->nonterminals[i]);
  }

  log_trace("Terminals");
  for (int i = 0; i < ptp->terminalCount; i++) {
    log_trace("%s", ptp->terminals[i]);
  }

  return ptp;
}

char *primitives_strings[] = {"void", "char",  "short", "int",
                              "long", "float", "double"};

#define PRIMITIVE_COUNT 7

bool isStringAPrimitive(const char *content) {
  for (int i = 0; i < PRIMITIVE_COUNT; i++) {
    if (strcmp(content, primitives_strings[i]) == 0) {
      return true;
    }
  }

  return false;
}

int main(void) {
  ParsingTable *parsingTable = loadParsingTable();
  Token tokens[10000];
  char *buffer = (char *)get_memory(2000 * sizeof(char));
  loadFileToBuffer("test.c", buffer);

  int tokenCount = lexer(buffer, tokens, arr_c_lexicon, arr_c_token_types,
                         C_TOKEN_TYPES_COUNT);
  // free(buffer);
  log_trace("Token count: %d", tokenCount);

  char **stack = get_memory(sizeof(char *) * tokenCount * tokenCount);
  stack[0] = "$";
  stack[1] = "start";

  int stackCounter = 1;
  int tokenCounter = 0;
  int errorCount = 0;

  bool isEnum = false;
  bool isPrimitive = false;

  bool foo_bool = true;

  Token *endToken = get_memory(sizeof(Token));
  endToken->content = "$";
  endToken->type = COUNT;
  tokens[tokenCount++] = *endToken;

  while (stackCounter > 0) {
    log_debug("--------------------------------------------------------");
    log_debug("--------------------------------------------------------");

    char stack_output[2000] = "";
    sprintf(stack_output, "%sstackCounter: %d, stack: ", stack_output,
            stackCounter);
    for (int i = 0; i < stackCounter; i++) {
      sprintf(stack_output, "%s\"%s\"", stack_output, stack[i]);
      if (i != stackCounter - 1) {
        sprintf(stack_output, "%s, ", stack_output);
      }
    }
    log_debug("%s", stack_output);

    char *topStackItem = stack[stackCounter];
    StatementType topItemType = getItemTokenType(topStackItem, parsingTable);
    Token token = tokens[tokenCounter];
    char *tokenString = token.content;

    log_debug("Parsing token no. %d: \"%s\" of type %s", tokenCounter,
              tokenString, arr_c_token_type_strings[token.type]);
    char prev_output[2000] = "";
    sprintf(prev_output, "%sPrevious 10 tokens: \"", prev_output);
    for (int i = (tokenCounter - 10 >= 0) ? tokenCounter - 10 : tokenCounter;
         i < tokenCounter; i++) {
      if (tokens[i].type == NEWLINE) {
        sprintf(prev_output, "%snewline ", prev_output);
      } else if (tokens[i].type == WHITESPACE) {
        sprintf(prev_output, "%swhitespace ", prev_output);
      } else {
        sprintf(prev_output, "%s%s ", prev_output, tokens[i].content);
      }
    }
    log_debug("%s", prev_output);

    char next_output[2000] = "";
    sprintf(next_output, "%sNext 10 tokens: \"", next_output);
    for (int i = tokenCounter; i < tokenCounter + 10; i++) {
      if (tokens[i].type == NEWLINE) {
        sprintf(next_output, "%snewline ", next_output);
      } else if (tokens[i].type == WHITESPACE) {
        sprintf(next_output, "%swhitespace ", next_output);
      } else {
        sprintf(next_output, "%s%s ", next_output, tokens[i].content);
      }
    }
    log_debug("%s", next_output);

    if (strcmp(tokenString, "enum") == 0) {
      isEnum = true;
    }

    if (isStringAPrimitive(tokenString)) {
      isPrimitive = true;
    }

    if (token.type == IDENTIFIER) {
      tokenString = "IDENTIFIER";
    }

    if (token.type == STRING) {
      tokenString = "STRING_LITERAL";
    }

    if (token.type == KEYWORD && strcmp(tokenString, "case") == 0) {
      tokenString = "CASE";
    }

    if (token.type == KEYWORD && strcmp(tokenString, "break") == 0) {
      tokenString = "BREAK";
    }

    if (token.type == KEYWORD && strcmp(tokenString, "default") == 0) {
      tokenString = "DEFAULT";
    }

    if (token.type == KEYWORD && strcmp(tokenString, "return") == 0) {
      tokenString = "RETURN";
    }

    if (token.type == INTEGER_CONST || token.type == FLOAT_CONST ||
        token.type == CHAR) {
      tokenString = "CONSTANT";
    }

    if (token.type == MULTILINE_COMMENT || token.type == SINGLELINE_COMMENT ||
        token.type == PREPROCESSOR || token.type == NEWLINE ||
        token.type == WHITESPACE) {
      log_debug("Ignoring token of type %s",
                arr_c_token_type_strings[token.type]);
      tokenCounter++;
      continue;
    }

    else if (strcmp(tokenString, "++") == 0) {
      tokenString = "INC_OP";
    }

    else if (strcmp(tokenString, "--") == 0) {
      tokenString = "DEC_OP";
    }

    if (strcmp(tokenString, "=") == 0 && isEnum) {
      tokenString = "ENUM_EQ";
    }

    if (strcmp(tokenString, "=") == 0 && !isEnum && !isPrimitive) {
      tokenString = "ASSIGN";
      if (foo_bool) {
        topStackItem = "assignment_operator";
        foo_bool = false;
      }
    }

    if (strcmp(tokenString, "=") == 0 && !isEnum && isPrimitive) {
      if (foo_bool) {
        topStackItem = "init_declarator_suffix";
        foo_bool = false;
      }
    }

    log_debug("Stack item \'%s\' of type: %s", topStackItem,
              topItemType == NONTERMINAL ? "NONTERMINAL" : "TERMINAL");

    log_debug("FLAGS: isEnum = %s, isPrimitive = %s\n",
              isEnum ? "true" : "false", isPrimitive ? "true" : "false");

    switch (topItemType) {
    case NONTERMINAL: {
      log_debug("case NONTERMINAL");

      int col = getTerminalIndexFromIndex(parsingTable, tokenString);
      int row = getNonTerminalIndexFromIndex(parsingTable, topStackItem);

      if (errorCount > 0) {
        row = getNonTerminalIndexFromIndex(parsingTable, "epsilon");
        errorCount = 0;
      }

      log_debug("Column: %d | token type: %s, token content: \"%s\"", col,
                arr_c_token_type_strings[token.type], tokenString);
      log_debug("Row: %d | Current stack item: %s", row, topStackItem);

      int tableIndex = (row * parsingTable->terminalCount) + col;
      log_debug("tableIndex: (%d * %d) + %d = %d", row,
                parsingTable->terminalCount, col, tableIndex);
      TableEntry *entry = parsingTable->table[tableIndex];

      log_debug("entry no. %d, { error: %s }", tableIndex,
                entry->error == true ? "true" : "false");

      if (entry->error == false) {
        log_debug("Starting to add items to stack...");
        log_debug("statementCount: %d", entry->production->statementCount);

        /* POP STUFF OFF THE STACK */
        stackCounter--;

        Production *prod = entry->production;
        for (int i = prod->statementCount - 1; i >= 0; i--) {
          Statement *statement = prod->statements[i];

          if (strcmp(statement->content, "epsilon") == 0) {
            log_trace("Epsilon, popping stack.");
            continue;
          }

          log_debug("Pushing \"%s\" to stack.", statement->content);

          stack[++stackCounter] = statement->content;
        }
      } else {
        log_error("Error on \'%s\' stack item and token \"%s\"!", topStackItem,
                  tokenString);
        log_debug("Error popping \'%s\' off the stack!", topStackItem);
        if (stackCounter <= 0) {
          log_debug("Error: stack is empty and no $ symbol is there.");
          log_error("Error, could not parse stack!");
          exit(EXIT_FAILURE);
        }
        stackCounter--;
      }
    }; break;
    case TERMINAL: {
      log_debug("case TERMINAL");
      log_debug("Comparing \"%s\" to \"%s\"", tokenString, topStackItem);
      if (strcmp(tokenString, topStackItem) == 0) {
        log_debug("popping \'%s\' off the stack.", topStackItem);

        if (strcmp(topStackItem, "ENUM_EQ") == 0) {
          isEnum = false;
        }
        if (strcmp(topStackItem, "ASSIGN") == 0 ||
            strcmp(topStackItem, "{") == 0) {
          isPrimitive = false;
          foo_bool = true;
        }

        /* POP STUFF OFF THE STACK */
        stackCounter--;
        errorCount = 0;
        if (tokenCounter < tokenCount - 1) {
          tokenCounter++;
        }
      } else {
        log_debug("Error popping \'%s\' off the stack!", topStackItem);
        if (stackCounter <= 0) {
          log_debug("Error: stack is empty and no $ symbol is there.");
          log_error("Error, could not parse stack!");
          exit(EXIT_FAILURE);
        }
        stackCounter--;
      }
    } break;

    default:
      break;
    }
  }

  log_debug("Stack counter at %d", stackCounter);
  return 0;
}

int getItemTokenType(char *item, ParsingTable *table) {
  log_trace("Getting token type of %s", item);
  for (int i = 0; i < table->nonterminalCount; i++) {
    if (strcmp(item, table->nonterminals[i]) == 0) {
      return NONTERMINAL;
    }
  }
  for (int i = 0; i < table->terminalCount; i++) {
    if (strcmp(item, table->terminals[i]) == 0) {
      return TERMINAL;
    }
  }

  log_error("Error: Parser, could not get token type of %s.", item);
  return -1;
}
