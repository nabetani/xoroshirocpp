#include "xoroshiro.h"
#include "xorshift128.h"
#include <chrono>
#include <cstdio>
#include <iostream>
#include <random>
#include <string>

template <typename rng_type> //
void test(char const *name, size_t n) {
  rng_type rng(1);
  std::uniform_int_distribution<int> dst_int(0, 100);
  int c = 0;
  using namespace std::chrono;
  using clock = steady_clock;
  auto t0 = clock::now();
  for (size_t ix = 0; ix < n; ++ix) {
    auto o = dst_int(rng);
    if (o == 0 || o == 100) {
      ++c;
    }
  }
  auto diff = clock::now() - t0;
  printf("%s: %fns  c=%.2f%%\n", name,
         duration_cast<nanoseconds>(diff).count() * 1.0 / n, c * 1e2 / n);
}

int main(int argc, char const *argv[]) {
  for (size_t n : {10000ull, 10000000ull}) {
    test<std::mt19937>("std::mt19937", n);
    test<xoroshiro::rng128pp>("xoroshiro::rng128pp", n);
    test<xoroshiro::rng128ss>("xoroshiro::rng128ss", n);
    test<xorshift128::rng>("xorshift128::rng", n);
  }
  return 0;
}