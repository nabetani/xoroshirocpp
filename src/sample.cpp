#include "xoroshiro.h"
#include <iostream>
#include <random>

template <typename proc_type> //
void run(char const *name, proc_type const &proc) {
  std::cout << "  " << name << ":";
  for (int i = 0; i < 10; ++i) {
    std::cout << " " << proc();
  }
  std::cout << "\n";
}

template <typename rng_type> void test(char const *name) {
  rng_type rng(0);
  std::uniform_int_distribution<int> dst_int(0, 100);
  std::uniform_real_distribution<double> dst_double(0, 100);
  std::normal_distribution<> normal(1, 2);
  std::cout << name << "\n";
  run("dst_int", [&]() { return dst_int(rng); });
  run("dst_double", [&]() { return dst_double(rng); });
  run("normal", [&]() { return normal(rng); });
  std::cout << std::endl;
}

int main(int argc, char const *argv[]) {
  test<std::mt19937>("std::mt19937");
  test<xoroshiro::rng128pp>("xoroshiro::rng128pp");
  test<xoroshiro::rng128ss>("xoroshiro::rng128ss");
  return 0;
}