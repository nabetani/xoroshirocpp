#include "TestU01.h"

unif01_Gen *create_unif01();

int main(void) {

  // Create TestU01 PRNG object for our generator
  unif01_Gen *gen = create_unif01();

  // Run the tests.
  bbattery_SmallCrush(gen);
  return 0;
}
