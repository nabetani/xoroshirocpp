#include <cstdint>

namespace xorshift128 {
struct rng {
public:
  /** The type of the generated random value. */
  using result_type = std::uint32_t;

  /** the smallest possible value in the output range. */
  static constexpr result_type min() { return 0; }

  /** the largest possible value in the output range. */
  static constexpr result_type max() { return ~(min()); }

  result_type x = 123456789, y = 362436069, z = 521288629, w = 88675123;
  explicit rng(result_type seed = 0) { z ^= seed; }
  result_type operator()() {
    result_type t = x ^ (x << 11);
    x = y;
    y = z;
    z = w;
    return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
  }
};
} // namespace xorshift128