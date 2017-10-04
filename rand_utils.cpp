#include <stdlib.h>
#include <time.h>  
#include "rand_utils.h"

int random(int n) {
  static bool has_seeded = false;
  if (!has_seeded) {
    srand (time(NULL));
    has_seeded = true;
  }
  return rand()%(n+1);
}
