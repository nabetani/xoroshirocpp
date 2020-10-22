#include "xoroshiro.h"
#include <iostream>
#include <random>

template <typename rng_type> void test(char const *name) {
  rng_type rng(0);
  std::uniform_int_distribution<int> dst_int(0, 100);
  std::uniform_real_distribution<double> dst_double(0, 100);
  std::normal_distribution<> normal(1, 2);
  std::cout                                            //
      << name << "\n"                                  //
      << "    dst_int: " << dst_int(rng) << "\n"       //
      << "    dst_double: " << dst_double(rng) << "\n" //
      << "    normal: " << normal(rng) << std::endl;   //
}

int main(int argc, char const *argv[]) {
  test<std::mt19937>("std::mt19937");
  test<xoroshiro::rng128pp>("xoroshiro128pp::rng");
  return 0;
}