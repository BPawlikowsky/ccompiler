#include "../types.h"
#include "../utils/utils.h"
#include "../memory.h"

char *get_first_set_string(FirstSet *set);
char *get_follow_set_string(FollowSet *set);
bool doesSetContainEpsilon(FirstSet *set);
