#include "settools.h"

char *get_first_set_string(FirstSet *set) {
  char *result = get_memory(sizeof(char) * set->itemCount * 50);
  for (int i = 0; i < set->itemCount; i++) {
    if (i < set->itemCount - 1)
      sprintf(result, "%s \'%s\' |", result, set->set[i]);
    else
      sprintf(result, "%s \'%s\'", result, set->set[i]);
  }

  return result;
}

char *get_follow_set_string(FollowSet *set) {
  char *result = get_memory(sizeof(char) * set->itemCount * 50);
  for (int i = 0; i < set->itemCount; i++) {
    if (i < set->itemCount - 1)
      sprintf(result, "%s \'%s\' |", result, set->set[i]);
    else
      sprintf(result, "%s \'%s\'", result, set->set[i]);
  }

  return result;
}

bool doesSetContainEpsilon(FirstSet *set) {
  for (int i = 0; i < set->itemCount; i++) {
    if (strcmp(set->set[i], "epsilon") == 0) {
      return true;
    }
  }

  return false;
}
