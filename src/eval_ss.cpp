#include <cstdio>
#include <cstdlib>
#include <random>

#include "TestU01.h"
#include "xoroshiro.h"

using rng_type = xoroshiro::rng128ss;

double double01(void *param, void *state) {
  rng_type *rng = (rng_type *)param;
  std::uniform_real_distribution<double> dist;
  return dist(*rng);
}

using bits_type = unsigned long;

bits_type bits(void *param, void *state) {
  rng_type *rng = (rng_type *)param;
  std::uniform_int_distribution<bits_type> dist(0, ~bits_type(0));
  return dist(*rng);
}

void write_name(void *state) { std::puts("xoroshiro128**"); }

extern "C" {
unif01_Gen *create_unif01();
}

unif01_Gen *create_unif01() {
  unif01_Gen *gen = new unif01_Gen;
  rng_type *rng = new rng_type;
  static char name[] = "xoroshiro128**";
  gen->name = name;
  gen->param = rng;
  gen->state = nullptr;
  gen->Write = write_name;
  gen->GetBits = bits;
  gen->GetU01 = double01;
  return gen;
}
