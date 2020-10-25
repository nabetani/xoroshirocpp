/**
 * @file xoshiro.h
 * @author NABETANI Takenori
 * @brief Xoshiro128** and Xoshiro128++ as c++ random lib.
 * @note
 *  algorithms are copied from following URLs:
 *    http://prng.di.unimi.it/xoshiro128plusplus.c
 */

#include <cstdint>
#include <istream>
#include <ostream>

#if defined __GNUC__ && defined __x86_64__
#include <x86intrin.h>
#endif

namespace xoshiro {

/** Namespace containing implementation details */
namespace detail {

/** rotate right (64bit uint)
 * @param x rotate this value
 * @param k rotate k bits
 * @return rotated value
 */
inline std::uint32_t rotl(std::uint32_t const x, int k) {
#if defined __GNUC__ && defined __x86_64__
  return __rold(x, k);
#else
  return (x << k) | (x >> (31 & -k));
#endif
}

/** create seed value#0
 * @param s source of seed
 * @return seed value
 * @note This is an nfounded calculation.
 */
inline std::uint32_t conv_seed(std::uint64_t s, int ix) {
  std::uint32_t const s0 =
      static_cast<std::uint32_t>((ix & 1) ? (s & 0xffffffff) : (s >> 32));
  std::uint32_t const s1 = rotl(s0, (ix & 2) ? 16 : 0);
  std::uint32_t const m = 1u << (31 & (s >> (5 * ix)));
  std::uint32_t const muls[] = {
      297968085u,
      1620086527u,
      1521909871u,
      1042423439u,
  };
  std::uint32_t const pats[] = {2617986876u, 1538821552u, 795482552u,
                                435381156u};
  return (s1 * muls[ix] ^ pats[ix]) | m;
}

/** base class of xoshiro128++ and xoshiro128**
 * @tparam derived derived class
 */
template <typename derived> class rng128base {
public:
  /** The type of the generated random value. */
  using result_type = std::uint32_t;

private:
  /** default rng seed #0
   * @note This is an nfounded value.
   */
  static result_type default_seed(size_t ix) {
    constexpr result_type seeds[] = {3169277489u, 915989341u, 1028940276u,
                                     952680402u};
    return seeds[ix];
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
    is >> rng.s[2];
    is >> rng.s[3];
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
    return os                 //
           << rng.s[0] << " " //
           << rng.s[1] << " " //
           << rng.s[2] << " " //
           << rng.s[3];
  }

protected:
  std::uint32_t s[4]; ///< inner rng state

  /** compares two rng objects of the same type for equality.
   * @param a rng object.
   * @param b another rng object.
   * @returns true if a and b generates same values.
   */
  bool same_to(rng128base const &that) const {
    return s[0] == that.s[0]     //
           && s[1] == that.s[1]  //
           && s[2] == that.s[2]  //
           && s[3] == that.s[3]; //
  }

  /** constructor with default seed. */
  rng128base()
      : s{
            default_seed(0),
            default_seed(1),
            default_seed(2),
            default_seed(3),
        } {}

  /** constructor with seed
   * @param seed seed of rng.
   */
  explicit rng128base(std::uint64_t seed)
      : s{detail::conv_seed(seed, 0), detail::conv_seed(seed, 1),
          detail::conv_seed(seed, 2), detail::conv_seed(seed, 3)} {}
};
} // namespace detail

/** Xoshiro128++ */
class rng128pp : public detail::rng128base<rng128pp> {
  using base = detail::rng128base<rng128pp>;

public:
  /** create new random number and update inner state.
   * @return new random number
   */
  result_type operator()() {
    result_type const result = detail::rotl(s[0] + s[3], 7) + s[0];
    result_type const t = s[1] << 9;
    s[2] ^= s[0];
    s[3] ^= s[1];
    s[1] ^= s[2];
    s[0] ^= s[3];
    s[2] ^= t;
    s[3] = detail::rotl(s[3], 11);
    return result;
  }

  /** constructor with seed
   * @param seed seed of rng.
   */
  explicit rng128pp(result_type seed) : base(seed) {}

  /** constructor with default seed. */
  rng128pp() : base() {}
};

/** Xoshiro128** */
class rng128ss : public detail::rng128base<rng128ss> {
  using base = detail::rng128base<rng128ss>;

public:
  /** create new random number and update inner state.
   * @return new random number
   */
  result_type operator()() {
    result_type const result = detail::rotl(s[1] * 5, 7) * 9;
    result_type const t = s[1] << 9;
    s[2] ^= s[0];
    s[3] ^= s[1];
    s[1] ^= s[2];
    s[0] ^= s[3];
    s[2] ^= t;
    s[3] = detail::rotl(s[3], 11);
    return result;
  }

  /** constructor with seed
   * @param seed seed of rng.
   */
  explicit rng128ss(result_type seed) : base(seed) {}

  /** constructor with default seed. */
  rng128ss() : base() {}
};
} // namespace xoshiro
