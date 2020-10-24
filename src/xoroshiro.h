/**
 * @file xoroshiro.h
 * @author NABETANI Takenori
 * @brief Xoroshiro128** and Xoroshiro128++ as c++ random lib.
 * @note
 *  algorithms are copied from following URLs:
 *    http://prng.di.unimi.it/xoroshiro128starstar.c
 *    http://prng.di.unimi.it/xoroshiro128plusplus.c
 *
 */

#include <cstdint>
#include <istream>
#include <ostream>

#if defined __GNUC__ && defined __x86_64__
#include <x86intrin.h>
#endif

namespace xoroshiro {

/** Namespace containing implementation details */
namespace detail {

/** rotate right (64bit uint)
 * @param x rotate this value
 * @param k rotate k bits
 * @return rotated value
 */
inline std::uint64_t rotl(const uint64_t x, int k) {
#if defined __GNUC__ && defined __x86_64__
  return __rolq(x, k);
#else
  return (x << k) | (x >> (64 - k));
#endif
}

/** create seed value#0
 * @param s source of seed
 * @return seed value
 * @note This is an nfounded calculation.
 */
inline std::uint64_t conv_seed0(std::uint64_t s) {
  return ((11660880167232069161ull * s) ^ 13827146352468370501ull) |
         (1ull << (s & 63));
}

/** create seed value#1
 * @param s source of seed
 * @return seed value
 * @note This is an nfounded calculation.
 */
inline std::uint64_t conv_seed1(std::uint64_t s) {
  return ((11559677109961209133ull * rotl(s, 32)) ^ 17519439474968054641ull) |
         (1ull << ((s / 64) & 63));
}

/** base class of xoroshiro128++ and xoroshiro128**
 * @tparam derived derived class
 */
template <typename derived> class rng128base {
public:
  /** The type of the generated random value. */
  using result_type = std::uint64_t;

private:
  /** default rng seed #0
   * @note This is an nfounded value.
   */
  static constexpr result_type default_seed0() {
    return 17804420534016853344ull;
  }

  /** default rng seed #1
   * @note This is an nfounded value.
   */
  static constexpr result_type default_seed1() {
    return 7735267388770358179ull;
  }

public:
  /** the smallest possible value in the output range. */
  static constexpr result_type min() { return 0; }

  /** the largest possible value in the output range. */
  static constexpr result_type max() { return ~(min()); }

  /** compares two rng objects of the same type for equality.
   * @param a rng object.
   * @param b another rng object.
   * @returns true if a and b generates same values.
   */
  inline friend //
      bool
      operator==(derived const &a, derived const &b) {
    return a.same_to(b);
  }

  /** compares two rng objects of the same type for equality.
   * @param a rng object.
   * @param b another rng object.
   * @returns false if a and b generates same values.
   */
  inline friend //
      bool
      operator!=(derived const &a, derived const &b) {
    return !a.same_to(b);
  }

  /** read state of rng from input stream
   * @tparam CharT a template parameter to specify stream class
   * @tparam Traits a template parameter to specify stream class
   * @param is An input stream.
   * @param rng read state from is and write to rng
   * @returns The input stream with the state of rng extracted.
   */
  template <typename CharT, typename Traits> //
  friend inline                              //
      std::basic_istream<CharT, Traits> &
      operator>>(std::basic_istream<CharT, Traits> &is, rng128base &rng) //
  {
    is >> rng.s[0];
    is >> rng.s[1];
    return is;
  }

  /** output current state of rng
   * @tparam CharT a template parameter to specify stream class
   * @tparam Traits a template parameter to specify stream class
   * @param os an output stream
   * @param rng output state of this value
   * @return the output stream with the state of rng inserted.
   */
  template <typename CharT, typename Traits> //
  friend inline                              //
      std::basic_ostream<CharT, Traits> &
      operator<<(std::basic_ostream<CharT, Traits> &os,
                 rng128base const &rng) //
  {
    return os << rng.s[0] << " " << rng.s[1];
  }

protected:
  std::uint64_t s[2]; ///< inner rng state

  /** compares two rng objects of the same type for equality.
   * @param a rng object.
   * @param b another rng object.
   * @returns true if a and b generates same values.
   */
  bool same_to(rng128base const &that) const {
    return s[0] == that.s[0] && s[1] == that.s[1];
  }

  /** constructor with default seed. */
  rng128base() : s{default_seed0(), default_seed1()} {}

  /** constructor with seed
   * @param seed seed of rng.
   */
  explicit rng128base(result_type seed)
      : s{detail::conv_seed0(seed), detail::conv_seed1(seed)} {}
};

} // namespace detail

/** Xoroshiro128++ */
class rng128pp : public detail::rng128base<rng128pp> {
  using base = detail::rng128base<rng128pp>;

public:
  /** create new random number and update inner state.
   * @return new random number
   */
  result_type operator()() {
    result_type const s0 = s[0];
    result_type s1 = s[1];
    uint64_t const result = detail::rotl(s0 + s1, 17) + s0;
    s1 ^= s0;
    s[0] = detail::rotl(s0, 49) ^ s1 ^ (s1 << 21); // a, b
    s[1] = detail::rotl(s1, 28);                   // c
    return result;
  }

  /** constructor with seed
   * @param seed seed of rng.
   */
  explicit rng128pp(result_type seed) : base(seed) {}

  /** constructor with default seed. */
  rng128pp() : base() {}
};

/** Xoroshiro128** */
class rng128ss : public detail::rng128base<rng128ss> {
  using base = detail::rng128base<rng128ss>;

public:
  /** create new random number and update inner state.
   * @return new random number
   */
  result_type operator()() {
    const uint64_t s0 = s[0];
    uint64_t s1 = s[1];
    const uint64_t result = detail::rotl(s0 * 5, 7) * 9;
    s1 ^= s0;
    s[0] = detail::rotl(s0, 24) ^ s1 ^ (s1 << 16); // a, b
    s[1] = detail::rotl(s1, 37);                   // c
    return result;
  }

  /** constructor with seed
   * @param seed seed of rng.
   */
  explicit rng128ss(result_type seed) : base(seed) {}

  /** constructor with default seed. */
  rng128ss() : base() {}
};
} // namespace xoroshiro
