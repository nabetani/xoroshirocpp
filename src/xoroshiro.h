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

template <typename derived> class rng128base {
public:
  using result_type = std::uint64_t;

  static constexpr result_type min() { return 0; }

  static constexpr result_type max() { return ~(min()); }

  inline friend bool operator==(derived const &a, derived const &b) {
    return a.same_to(b);
  }

protected:
  std::uint64_t s[2];

  bool same_to(rng128base const &that) const {
    return s[0] == that.s[0] && s[1] == that.s[1];
  }
  rng128base() = delete;

  explicit rng128base(result_type seed)
      : s{detail::conv_seed0(seed), detail::conv_seed1(seed)} {}
};

} // namespace detail
class rng128pp : public detail::rng128base<rng128pp> {
  using base = detail::rng128base<rng128pp>;

public:
  result_type operator()() {
    result_type const s0 = s[0];
    result_type s1 = s[1];
    uint64_t const result = detail::rotl(s0 + s1, 17) + s0;
    s1 ^= s0;
    s[0] = detail::rotl(s0, 49) ^ s1 ^ (s1 << 21); // a, b
    s[1] = detail::rotl(s1, 28);                   // c
    return result;
  }
  explicit rng128pp(result_type seed) : base(seed) {}
};

class rng128ss : public detail::rng128base<rng128ss> {
  using base = detail::rng128base<rng128ss>;

public:
  result_type operator()() {
    uint64_t const s0 = s[0];
    uint64_t const result = detail::rotl(s0 * 5, 7) * 9;
    uint64_t const s1 = s0 ^ s[1];
    s[0] = detail::rotl(s0, 24) ^ s1 ^ (s1 << 16); // a, b
    s[1] = detail::rotl(s1, 37);                   // c
    return result;
  }
  explicit rng128ss(result_type seed) : base(seed) {}
};
} // namespace xoroshiro
