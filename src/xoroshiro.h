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
#include <istream>
#include <ostream>

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

private:
  static constexpr result_type default_seed0() {
    return 17804420534016853344ull;
  }
  static constexpr result_type default_seed1() {
    return 7735267388770358179ull;
  }

public:
  static constexpr result_type min() { return 0; }

  static constexpr result_type max() { return ~(min()); }

  inline friend //
      bool
      operator==(derived const &a, derived const &b) {
    return a.same_to(b);
  }
  inline friend //
      bool
      operator!=(derived const &a, derived const &b) {
    return !a.same_to(b);
  }

  template <typename CharT, typename Traits> //
  friend inline                              //
      std::basic_istream<CharT, Traits> &
      operator>>(std::basic_istream<CharT, Traits> &is, rng128base &rng) //
  {
    is >> rng.s[0];
    is >> rng.s[1];
    return is;
  }

  template <typename CharT, typename Traits> //
  friend inline                              //
      std::basic_ostream<CharT, Traits> &
      operator<<(std::basic_ostream<CharT, Traits> &os,
                 rng128base const &rng) //
  {
    return os << rng.s[0] << " " << rng.s[1];
  }

protected:
  std::uint64_t s[2];

  bool same_to(rng128base const &that) const {
    return s[0] == that.s[0] && s[1] == that.s[1];
  }
  rng128base() : s{default_seed0(), default_seed1()} {}

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
  rng128pp() : base() {}
};

class rng128ss : public detail::rng128base<rng128ss> {
  using base = detail::rng128base<rng128ss>;

public:
  result_type operator()() {
    const uint64_t s0 = s[0];
    uint64_t s1 = s[1];
    const uint64_t result = detail::rotl(s0 * 5, 7) * 9;
    s1 ^= s0;
    s[0] = detail::rotl(s0, 24) ^ s1 ^ (s1 << 16); // a, b
    s[1] = detail::rotl(s1, 37);                   // c
    return result;
  }
  explicit rng128ss(result_type seed) : base(seed) {}
  rng128ss() : base() {}
};
} // namespace xoroshiro
