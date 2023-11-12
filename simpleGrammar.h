/*
 * E -> T + E | T
 * T -> int | int * T | (E)
 * Recursive Descent Algorithm
*/
Statement def1prod1st1 = {
          .type = 1,
          .content = "T"
          };
Statement def1prod1st2 = {
          .type = 0,
          .content = "+"
          };
Statement def1prod1st3 = {
          .type = 1,
          .content = "E"
          };
Production def1prod1 = {
        .statementCount = 3,
        .statements = {
          &def1prod1st1,
          &def1prod1st2,
          &def1prod1st3
        }
    };
Statement def1prod2st1 = {
          .type = 1,
          .content = "T"
          };
Production def1prod2 = {
        .statementCount = 1,
        .statements = {
          &def1prod2st1
        }
    };
Definition def1 = {
    .name = "E",
    .productionCount = 2,
    .productions = { &def1prod1, &def1prod2 }
  };

Statement def2prod1st1 = {
          .type = 0,
          .content = "int"
          };
Production def2prod1 = {
        .statementCount = 1,
        .statements = {
          &def2prod1st1
        }
    };
Statement def2prod2st1 = {
          .type = 0,
          .content = "int"
          };
Statement def2prod2st2 = {
          .type = 0,
          .content = "*"
          };
Statement def2prod2st3 = {
          .type = 1,
          .content = "T"
          };
Production def2prod2 = {
        .statementCount = 3,
        .statements = {
          &def2prod2st1,
          &def2prod2st2,
          &def2prod2st3
        }
    };
Statement def2prod3st1 = {
          .type = 0,
          .content = "("
          };
Statement def2prod3st2 = {
          .type = 1,
          .content = "E"
          };
Statement def2prod3st3 = {
          .type = 0,
          .content = ")"
          };
Production def2prod3 = {
        .statementCount = 3,
        .statements = {
          &def2prod3st1,
          &def2prod3st2,
          &def2prod3st3
        }
    };
Definition def2 = {
    .name = "T",
    .productionCount = 3,
    .productions = { &def2prod1, &def2prod2, &def2prod3 }
  };
Definition *definitions1[] = { &def1, &def2 };
char *defs1[] = { "E", "T" };