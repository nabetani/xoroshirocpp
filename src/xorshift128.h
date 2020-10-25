#include <cstdint>
#include <istream>
#include <ostream>

namespace xorshift128 {
struct rng {
public:
  /** The type of the generated random value. */
  using result_type = std::uint32_t;

  /** the smallest possible value in the output range. */
  static constexpr result_type min() { return 0; }

  /** the largest possible value in the output range. */
  static constexpr result_type max() { return ~(min()); }

  result_type x = 123456789;
  result_type y = 362436069;
  result_type z = 521288629;
  result_type w = 88675123;
  explicit rng(result_type seed = 0) { z ^= seed; }
  result_type operator()() {
    result_type t = x ^ (x << 11);
    x = y;
    y = z;
    z = w;
    return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
  }
  template <typename CharT, typename Traits> //
  friend inline                              //
      std::basic_ostream<CharT, Traits> &
      operator<<(std::basic_ostream<CharT, Traits> &os,
                 rng const &o) //
  {
    return os            //
           << o.x << " " //
           << o.y << " " //
           << o.z << " " //
           << o.w;
  }
  friend inline bool operator==(rng const &a, rng const &b) {
    return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
  }
  friend inline bool operator!=(rng const &a, rng const &b) {
    return !(a == b);
  }
  template <typename CharT, typename Traits> //

  friend inline //
      std::basic_istream<CharT, Traits> &
      operator>>(std::basic_istream<CharT, Traits> &is, rng &o) //
  {
    is >> o.x;
    is >> o.y;
    is >> o.z;
    is >> o.w;
    return is;
  }
};
} // namespace xorshift128