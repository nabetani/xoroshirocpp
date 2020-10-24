#include "xoroshiro.h"
#include "xorshift128.h"
#include <iostream>
#include <random>
#include <sstream>
#include <string>

template <typename proc_type> //
void run(char const *name, proc_type const &proc) {
  std::cout << "  " << name << ":";
  for (int i = 0; i < 10; ++i) {
    std::cout << " " << proc();
  }
  std::cout << "\n";
}

template <typename obj_type> //
obj_type copy_by_stream(obj_type const &o) {
  std::stringstream ss0;
  ss0 << o;
  auto str = ss0.str();
  std::istringstream ss1{str};
  obj_type r;
  ss1 >> r;
  return r;
}

template <typename rng_type> //
void test(char const *name) {
  rng_type rng(0);
  std::uniform_int_distribution<int> dst_int(0, 100);
  std::uniform_real_distribution<double> dst_double(0, 100);
  std::normal_distribution<> normal(1, 2);
  std::cout << name << "(" << rng << ")\n";
  run("dst_int", [&]() { return dst_int(rng); });
  run("dst_double", [&]() { return dst_double(rng); });
  run("normal", [&]() { return normal(rng); });

  rng_type copy = rng;
  std::cout << (rng == copy ? "ok" : "***NG***") << std::endl;
  std::cout << (dst_int(rng) == dst_int(copy) ? "ok" : "***NG***") << std::endl;
  dst_int(rng);
  std::cout << (dst_int(rng) != dst_int(copy) ? "ok" : "***NG***") << std::endl;

  rng_type copy2 = copy_by_stream<rng_type>(rng);
  std::cout << (rng == copy2 ? "ok" : "***NG***") << std::endl;

  std::cout << std::endl;
}

int main(int argc, char const *argv[]) {
  test<std::mt19937>("std::mt19937");
  test<xoroshiro::rng128pp>("xoroshiro::rng128pp");
  test<xoroshiro::rng128ss>("xoroshiro::rng128ss");
  test<xorshift128::rng>("xorshift128::rng");
  return 0;
}