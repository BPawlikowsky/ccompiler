#include "parserGenerator.h"

int main() {
  Token tokens[1000];
  char *buffer = (char *)malloc(20000 * sizeof(char));
  loadFileToBuffer("./c89_mod.txt", buffer);

  int tokenCount =
      lexer(buffer, tokens, lexicon, tokenTypes, TOKEN_TYPES_COUNT);
  free(buffer);
  log_info("Token count: %d", tokenCount);

  // Count definitions and productions
  int it = 0;
  int defCount = 0;
  int prodCount = 0;

  while (it < tokenCount) {
    if (tokens[it].type == PROD)
      prodCount++;
    if (tokens[it].type == DEF)
      defCount++;
    it++;
  }

  log_info("Definitions: %d", defCount);
  log_info("Productions: %d", prodCount);

  // Parse definitions and productions
  Definition **definitions = malloc(sizeof(Definition *) * defCount);

  char *defs[defCount];
  it = 0;
  int id = 0;
  int ip = 0;

  while (it < tokenCount) {
    // Parse definitions
    if (tokens[it].type == DEF) {
      // Trim definition string
      char *fp = strstr(tokens[it].content, ":");
      tokens[it].content[fp - tokens[it].content] = '\0';

      // Add def to definitions array
      defs[id] = tokens[it].content;
      Definition *definition = malloc(sizeof(Definition));
      definition->productions = malloc(sizeof(Production) * prodCount);

      definition->name = tokens[it].content;
      definition->productionCount = 0;
      definitions[id] = definition;
      ip = 0;
      id++;
    }
    // Parse productions
    if (tokens[it].type == PROD) {
      Production *prod = malloc(sizeof(Production));
      prod->statements = malloc(sizeof(Statement) * 100);

      trimProdStrings(tokens, it);
      prod->statementCount = 0;

      definitions[id - 1]->productions[ip] = prod;
      addStatements(definitions[id - 1]->productions[ip], tokens, it);
      definitions[id - 1]->productionCount++;
      ip++;
    }
    it++;
  }

  addTypeToStatements(definitions, defCount, defs);

  log_info("*** DEFINITIONS ***");
  printDefs(definitions, defCount);

  FirstSet **firstSets = malloc(sizeof(FirstSet *) * defCount);
  FollowSet **followSets = malloc(sizeof(FollowSet *) * defCount);
  FollowSet *startSet = malloc(sizeof(FollowSet));

  // ADDING $ SYMBOL TO START FOLLOW SET
  startSet->itemCount = 1;
  startSet->set = malloc(sizeof(char *));
  startSet->set[0] = malloc(sizeof(char) * 2);
  startSet->set[0] = "$";

  followSets[0] = startSet;

  GeneratorState genState = {.defs = defs,
                             .definitions = definitions,
                             .defCount = defCount,
                             .historyCounter = 0,
                             .firstSetCounter = 0,
                             .firstSets = firstSets,
                             .followSetCounter = 1,
                             .followSets = followSets};

  char **terminals = malloc(sizeof(char *) * 150);
  char **nonterminals = malloc(sizeof(char *) * 150);
  int terminalCount = getTerminals(terminals, &genState);
  int nonterminalCount = getNonTerminals(nonterminals, &genState);

  genState.terminals = terminals;
  genState.terminalCount = terminalCount;
  genState.nonterminals = nonterminals;
  genState.nonterminalCount = nonterminalCount;

  FirstSetHistory *first_set_history = malloc(sizeof(FirstSetHistory));
  first_set_history->arr_sets = malloc(sizeof(char **) * defCount);
  first_set_history->arr_visited_count = malloc(sizeof(int *) * defCount);

  for (int i = 0; i < defCount; i++) {
    first_set_history->arr_visited_count[i] = malloc(sizeof(int));
    *first_set_history->arr_visited_count[i] = 0;
  }

  genState.first_set_history = first_set_history;

  FollowSetHistory *follow_set_history = malloc(sizeof(FollowSetHistory));
  follow_set_history->arr_sets = malloc(sizeof(char **) * defCount);
  follow_set_history->arr_visited_count = malloc(sizeof(int *) * defCount);

  for (int i = 0; i < defCount; i++) {
    follow_set_history->arr_visited_count[i] = malloc(sizeof(int));
    *follow_set_history->arr_visited_count[i] = 0;
  }

  genState.follow_set_history = follow_set_history;

  follow_set_history->arr_sets[0] = startSet;

  for (int defIndex = 0; defIndex < defCount; defIndex++) {
    FirstSet *res = getFirstSet(defIndex, &genState);
  }

  // SKIPPING START FOLLOW SET
  for (int defIndex = 1; defIndex < defCount; defIndex++) {
    FollowSet *res = getFollowSet(defIndex, &genState);
  }

  // PRINT OR SAVE
  {
    char *outputbuffer = malloc(sizeof(char) * 100000);
    char *p = outputbuffer;

    for (int i = 0; i < genState.firstSetCounter; i++) {
      if (genState.firstSets[i] != NULL) {
        sprintf(outputbuffer, "%sFirstSet(%d) \'%s\': %s\n", outputbuffer, i,
                definitions[i]->name,
                get_first_set_string(genState.firstSets[i]));
        log_trace("FirstSet(%d) \'%s\': %s", i, definitions[i]->name,
                  get_first_set_string(genState.firstSets[i]));
        log_trace("FirstSet(%d) count: %d", i,
                  genState.firstSets[i]->itemCount);
      } else
        log_trace("FirstSet(%d) \'%s\': NULL", i, definitions[i]->name);
    }

    FILE *fptr;

    fptr = fopen("firstset.txt", "w");

    fprintf(fptr, "%s", outputbuffer);

    fclose(fptr);
    free(outputbuffer);
  }
  {
    char *outputbuffer = malloc(sizeof(char) * 100000);
    char *p = outputbuffer;

    for (int i = 0; i < genState.followSetCounter; i++) {
      if (genState.followSets[i] != NULL) {
        sprintf(outputbuffer, "%sFollowSet(%d) \'%s\': %s\n", outputbuffer, i,
                definitions[i]->name,
                get_follow_set_string(genState.followSets[i]));
        log_trace("FollowSet(%d) \'%s\': %s", i, definitions[i]->name,
                  get_follow_set_string(genState.followSets[i]));
        log_trace("FollowSet(%d) count: %d", i,
                  genState.followSets[i]->itemCount);
      } else
        log_trace("FollowSet(%d) \'%s\': NULL", i, definitions[i]->name);
    }

    FILE *fptr;

    fptr = fopen("followset.txt", "w");

    fprintf(fptr, "%s", outputbuffer);

    fclose(fptr);
    free(outputbuffer);
  }

  // printDefSymbols(&genState);

  ParsingTable *table = createParsingTable(&genState);

  saveParsingTableAsCSV(table);

  saveParsingTableAsBinaryData(table);

  saveTerminalsAndNonTerminalsAsText(table);

  return 0;
}

// HELPER FUNCTIONS

void addStatements(Production *prod, Token tokens[], int it) {
  char *p = tokens[it].content;
  char *statementsLeft = tokens[it].content;
  char *finish = &tokens[it].content[strlen(tokens[it].content)];
  int is = 0;
  while (p < finish) {
    Statement *statement = malloc(sizeof(Statement));
    assert(statement != NULL);
    char *statementEnd = strstr(statementsLeft, " ");

    if (statementEnd == NULL) {
      int strLen = strlen(statementsLeft);
      statement->content = (char *)malloc(sizeof(char) * strLen + 1);
      assert(statement->content != NULL);

      strcpy(statement->content, statementsLeft);
      int stringLength = strlen(statementsLeft);
      statement->content[stringLength] = '\0';
      statement->stringLength = stringLength + 1;

      // add single statement to production and break
      prod->statements[is++] = statement;
      break;
    }

    long offset = statementEnd - p;

    statement->content = (char *)malloc(sizeof(char) * (int)offset + 1);
    assert(statement->content != NULL);

    strncpy(statement->content, statementsLeft, offset);
    statement->content[offset] = '\0';
    statement->stringLength = offset + 1;
    // offset + 1 because of the null terminator
    statementsLeft += offset + 1;
    p += offset + 1;
    // add statement to production
    prod->statements[is++] = statement;
  }
  prod->statementCount = is;
}

StatementType getStatementType(char *defs[], char *statement, int defCount) {
  for (int i = 0; i < defCount; i++) {
    if (strcmp(defs[i], statement) == 0)
      return NONTERMINAL;
  }
  return TERMINAL;
}

void addTypeToStatements(Definition *defs[], int defCount, char *strDefs[]) {
  int i = 0;
  while (i < defCount) {
    int p = 0;
    while (p < defs[i]->productionCount) {
      int s = 0;
      while (s < defs[i]->productions[p]->statementCount) {
        Statement *statement = defs[i]->productions[p]->statements[s];
        statement->type =
            getStatementType(strDefs, statement->content, defCount);
        s++;
      }
      p++;
    }
    i++;
  }
}

void saveParsingTableAsCSV(ParsingTable *table) {
  // --- Save parsing table as csv table ---

  char *outputbuffer = malloc(sizeof(char) * 100000);
  char *p = outputbuffer;

  sprintf(p, "\"non-terminals\",");
  p += strlen("\"non-terminals\",");
  for (int i = 0; i < table->terminalCount; i++) {
    sprintf(p, "%s\"%s\",", p, table->terminals[i]);
    p += strlen(table->terminals[i]);
    p += 3;
  }

  sprintf(p, "%s\n", p);
  p++;

  sprintf(p, "%s\"%s\",", p, table->nonterminals[0]);
  p += strlen(table->nonterminals[0]);
  p += 3;

  int counter = 1;

  for (int i = 0; i < table->tableSize; i++) {
    TableEntry *entry = table->table[i];
    if (i > 0 && i % (table->terminalCount) == 0) {

      sprintf(p, "%s\n\"%s\",", p, table->nonterminals[counter]);
      p += strlen(table->nonterminals[counter]);
      p += 4;
      counter++;
    }
    if (entry->error == true) {
      sprintf(p, "\"error\",");
      p += strlen("\"error\",");
    } else {
      char *output = concatStatements(entry->production->statements,
                                      entry->production->statementCount, " ");
      sprintf(p, "\"%s\",", output);
      p += strlen(output);
      // offset to add quotes and comma ("",)
      p += 3;
    }
  }

  FILE *fptr;

  fptr = fopen("parsing-table.csv", "w");

  fprintf(fptr, "%s", outputbuffer);

  fclose(fptr);
  free(outputbuffer);

  // --- csv end ---
}

void saveParsingTableAsBinaryData(ParsingTable *table) {
  FILE *binFile = fopen("ptable.dat", "wb");

  // write ParsingTable
  int isFileWritten = fwrite(table, sizeof(ParsingTable), 1, binFile);

  // write TableEntry array
  for (int i = 0; i < table->tableSize; i++) {
    isFileWritten = fwrite(table->table[i], sizeof(TableEntry), 1, binFile);

    if (table->table[i]->error == false) {
      Production *production = table->table[i]->production;
      isFileWritten = fwrite(production, sizeof(Production), 1, binFile);

      for (int j = 0; j < production->statementCount; j++) {
        Statement *statement = table->table[i]->production->statements[j];
        isFileWritten = fwrite(statement, sizeof(Statement), 1, binFile);
        isFileWritten =
            fwrite(statement->content, sizeof(char) * statement->stringLength,
                   1, binFile);
      }
    }
  }

  fclose(binFile);
}

void saveTerminalsAndNonTerminalsAsText(ParsingTable *table) {
  FILE *file = fopen("nonterminals.txt", "w");
  for (int i = 0; i < table->nonterminalCount; i++) {
    int isFileWritten = fprintf(file, "%s\n", table->nonterminals[i]);
  }
  fclose(file);

  file = fopen("terminals.txt", "w");
  for (int i = 0; i < table->terminalCount; i++) {
    int isFileWritten = fprintf(file, "%s\n", table->terminals[i]);
  }
  fclose(file);
}