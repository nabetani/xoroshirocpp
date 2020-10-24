#include <cstdio>
#include <cstdlib>
#include <random>

#include "TestU01.h"

double double01_mt(void *param, void *state) {
  std::mt19937 *rng = (std::mt19937 *)param;
  std::uniform_real_distribution<double> dist;
  return dist(*rng);
}

using bits_type = unsigned long;

bits_type bits_mt(void *param, void *state) {
  std::mt19937 *rng = (std::mt19937 *)param;
  std::uniform_int_distribution<bits_type> dist(0, ~bits_type(0));
  return dist(*rng);
}

void write_mt(void *state) { std::puts("mt"); }

extern "C" {
unif01_Gen *create_unif01();
}

unif01_Gen *create_unif01() {
  unif01_Gen *gen = new unif01_Gen;
  std::mt19937 *rng = new std::mt19937;
  static char name[] = "mt19937";
  gen->name = name;
  gen->param = rng;
  gen->state = rng;
  gen->Write = write_mt;
  gen->GetBits = bits_mt;
  gen->GetU01 = double01_mt;
  return gen;
}
