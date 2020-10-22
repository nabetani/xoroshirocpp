/**
 * @file xoroshiross.h
 * @author NABETANI Takenori
 * @brief use Xoroshiro128** and Xoroshiro128++ as c++ random lib.
 * @note
 *  algorithms are copied frmo following URLs:
 *    http://prng.di.unimi.it/xoroshiro128starstar.c
 *    http://prng.di.unimi.it/xoroshiro128plusplus.c
 *
 */

#include <cstdint>
#include <limits>

namespace xoroshiro {
namespace detail {

inline std::uint64_t rotl(const uint64_t x, int k) {
  return (x << k) | (x >> (64 - k));
}
inline std::uint64_t conv_seed0(std::uint64_t s) {
  return (11660880167232069161ull * s) ^ 13827146352468370501ull;
}
inline std::uint64_t conv_seed1(std::uint64_t s) {
  return (11559677109961209133ull * rotl(s, 32)) ^ 17519439474968054641ull;
}
} // namespace detail
class rng128pp {

public:
  using result_type = std::uint64_t;

  /**
   * @brief Gets the smallest possible value in the output range.
   */
  static constexpr result_type min() { return 0; }

  /**
   * @brief Gets the largest possible value in the output range.
   */
  static constexpr result_type max() {
    return std::numeric_limits<result_type>::max();
  }
  result_type operator()() {
    result_type const s0 = s[0];
    result_type s1 = s[1];
    uint64_t const result = detail::rotl(s0 + s1, 17) + s0;
    s1 ^= s0;
    s[0] = detail::rotl(s0, 49) ^ s1 ^ (s1 << 21); // a, b
    s[1] = detail::rotl(s1, 28);                   // c
    return result;
  }

  friend bool operator==(rng128pp const &a, rng128pp const &b) {
    return a.s[0] == b.s[0] && a.s[1] == b.s[1];
  }

  std::uint64_t s[2];
  rng128pp() = delete;
  explicit rng128pp(result_type seed)
      : s{detail::conv_seed0(seed), detail::conv_seed1(seed)} {}
};
} // namespace xoroshiro
