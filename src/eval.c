#include "TestU01.h"

unif01_Gen *create_unif01();

enum { NDIM = 200 };
enum { CRUSH_NUM = 1 };

void mid_crush(unif01_Gen *gen);

int main(void) {

  // Create TestU01 PRNG object for our generator
  unif01_Gen *gen = create_unif01();

  // Run the tests.
  mid_crush(gen);
  return 0;
}
