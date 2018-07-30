//****************************************************************************//
//       Copyright (C) 2016 Florent Hivert <Florent.Hivert@lri.fr>,           //
//                                                                            //
//  Distributed under the terms of the GNU General Public License (GPL)       //
//                                                                            //
//    This code is distributed in the hope that it will be useful,            //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of          //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       //
//   General Public License for more details.                                 //
//                                                                            //
//  The full text of the GPL is available at:                                 //
//                                                                            //
//                  http://www.gnu.org/licenses/                              //
//****************************************************************************//

#ifndef HPCOMBI_PERM16_HPP_INCLUDED
#define HPCOMBI_PERM16_HPP_INCLUDED

#include <array>
#include <vector>
#include <cassert>
#include <cstdint>
#include <functional>  // less<>
#include <ostream>
#include <x86intrin.h>

#include "epu.hpp"

namespace HPCombi {


// Forward declaration
struct Perm16;
struct PTransf16;
struct Transf16;


  template <char IDX_MODE> inline uint64_t search_index(int bound) const;

  inline uint64_t last_non_zero(int bnd = Size()) const;
  inline uint64_t first_non_zero(int bnd = Size()) const;
  inline uint64_t last_zero(int bnd = Size()) const;
  inline uint64_t first_zero(int bnd = Size()) const;

  inline Vect16 eval16_ref() const;
  inline Vect16 eval16_vect() const;

  /** Test for partial transformation
   * @details
   * @returns whether \c *this is a partial transformation.
   * @param k the size of \c *this (default 16)
   *
   * Points where the function is undefined are mapped to \c 0xff. If \c *this
   * is a tranformation of @f$0\dots n-1@f$ for @f$n<16@f$, it should be completed
   * to a transformation of @f$0\dots 15@f$ by adding fixed points. That is the
   * values @f$i\geq n@f$ should be mapped to themself.
   * @par Example:
   * The partial tranformation
   * @f$\begin{matrix}0 1 2 3 4 5\\ 2 0 5 . . 4 \end{matrix}@f$
   * is encoded by the array {2,0,5,0xff,0xff,4,6,7,8,9,10,11,12,13,14,15}
   */
  inline bool is_partial_transformation(const size_t k = Size()) const;
  /** Test for transformation
   * @details
   * @returns whether \c *this is a transformation.
   * @param k the size of \c *this (default 16)
   *
   * If \c *this is a tranformation of @f$0\dots n-1@f$ for @f$n<16@f$,
   * it should be completed to a transformation of @f$0\dots 15@f$
   * by adding fixed points. That is the values @f$i\geq n@f$ should be
   * mapped to themself.
   * @par Example:
   * The tranformation
   * @f$\begin{matrix}0 1 2 3 4 5\\ 2 0 5 2 1 4 \end{matrix}@f$
   * is encoded by the array {2,0,5,2,1,4,6,7,8,9,10,11,12,13,14,15}
   */
  inline bool is_transformation(const size_t k = Size()) const;
  /** Test for permutations
   * @details
   * @returns whether \c *this is a permutation.
   * @param k the size of \c *this (default 16)
   *
   * If \c *this is a permutation of @f$0\dots n-1@f$ for @f$n<16@f$,
   * it should be completed to a permutaition of @f$0\dots 15@f$
   * by adding fixed points. That is the values @f$i\geq n@f$ should be
   * mapped to themself.
   * @par Example:
   * The permutation
   * @f$\begin{matrix}0 1 2 3 4 5\\ 2 0 5 3 1 4 \end{matrix}@f$
   * is encoded by the array {2,0,5,3,1,4,6,7,8,9,10,11,12,13,14,15}
   */
  inline bool is_permutation(const size_t k = Size()) const;

  inline static Vect16 random(uint16_t bnd = 256);

private:
  inline static const std::array<Perm16, 9> sorting_rounds();
  inline static const std::array<Perm16, 6> sorting_rounds8();
  inline static const std::array<epu8, 4> summing_rounds();
};

inline std::ostream &operator<<(std::ostream &stream, const Vect16 &term);


/** Partial transformation of @f$\{0\dots 15\}@f$
 *
 */
struct PTransf16 : public Vect16 {
  using vect = Vect16;

  PTransf16() = default;
  HPCOMBI_CONSTEXPR_CONSTRUCTOR PTransf16(const vect v) : vect(v) {}
  HPCOMBI_CONSTEXPR_CONSTRUCTOR PTransf16(const epu8 x) : vect(x) {}
  PTransf16(std::initializer_list<uint8_t> il);

  static HPCOMBI_CONSTEXPR PTransf16 one() { return make_epu8(make_one); }
  PTransf16 inline operator*(const PTransf16 &p) const {
    return permuted(p).v | (v == cst_epu8_0xFF); }

  uint32_t image() const;
};

/** Full transformation of @f$\{0\dots 15\}@f$
 *
 */
struct Transf16 : public PTransf16 {

  Transf16() = default;
  HPCOMBI_CONSTEXPR_CONSTRUCTOR Transf16(const vect v) : PTransf16(v) {}
  HPCOMBI_CONSTEXPR_CONSTRUCTOR Transf16(const epu8 x) : PTransf16(x) {}
  Transf16(std::initializer_list<uint8_t> il) : PTransf16(il) {}
  explicit Transf16(uint64_t compressed);

  explicit operator uint64_t() const;

  static HPCOMBI_CONSTEXPR Transf16 one() { return make_epu8(make_one); }
  Transf16 inline operator*(const Transf16 &p) const { return permuted(p); }
};


/** Permutations of @f$\{0\dots 15\}@f$
 *
 */
struct Perm16 : public Transf16 {

  Perm16() = default;
  HPCOMBI_CONSTEXPR_CONSTRUCTOR Perm16(const vect v) : Transf16(v) {}
  HPCOMBI_CONSTEXPR_CONSTRUCTOR Perm16(const epu8 x) : Transf16(x) {}
  Perm16(std::initializer_list<uint8_t> il) : Transf16(il) {}
  explicit Perm16(uint64_t compressed) : Transf16(compressed) {}
  Perm16 inline operator*(const Perm16 &p) const { return permuted(p); }

  /** @class common_inverse
   * @brief The inverse permutation
   * @details
   * @returns the inverse of \c *this
   * @par Example:
   * @code
   * Perm16 x = {0,3,2,4,1,5,6,7,8,9,10,11,12,13,14,15};
   * x.inverse()
   * @endcode
   * Returns {0,4,2,1,3,5,6,7,8,9,10,11,12,13,14,15}
   */
  /** @copydoc common_inverse
   *  @par Algorithm:
   *  Reference @f$O(n)@f$ algorithm using loop and indexed access
   */
  inline Perm16 inverse_ref() const;
  /** @copydoc common_inverse
   *  @par Algorithm:
   *  @f$O(n)@f$ algorithm using reference cast to arrays
   */
  inline Perm16 inverse_arr() const;
  /** @copydoc common_inverse
   *  @par Algorithm:
   *  Insert the identity in the least significant bits and sort using a
   *  sorting network. The number of round of the optimal sorting network is
   *  as far as I know open, therefore, the complexity is unknown.
   */
  inline Perm16 inverse_sort() const;
  /** @copydoc common_inverse
   *  @par Algorithm:
   *  @f$O(\log n)@f$ algorithm using some kind of vectorized dichotomic search.
   */
  inline Perm16 inverse_find() const;
  /** @copydoc common_inverse
   *  @par Algorithm:
   *
   * Raise \e *this to power @f$\text{LCM}(1, 2, ..., n) - 1@f$ so complexity
   * is in @f$O(log (\text{LCM}(1, 2, ..., n) - 1)) = O(n)@f$
   */
  inline Perm16 inverse_pow() const;
  /** @copydoc common_inverse
   *  @par Algorithm:
   *  Compute power from @f$n/2@f$ to @f$n@f$, when @f$\sigma^k(i)=i@f$ then
   *  @f$\sigma^{-1}(i)=\sigma^{k-1}(i)@f$. Complexity @f$O(n)@f$
   */
  inline Perm16 inverse_cycl() const;
  /** @copydoc common_inverse
   *
   *  Frontend method: currently aliased to #inverse_cycl */
  inline Perm16 inverse() { return inverse_cycl(); }

  // It's not possible to have a static constexpr member of same type as class
  // being defined (see https://stackoverflow.com/questions/11928089/)
  // therefore we chose to have functions.
  static HPCOMBI_CONSTEXPR
  Perm16 one() { return make_epu8(make_one); }
  static HPCOMBI_CONSTEXPR
  Perm16 left_cycle() { return make_epu8(make_left_cycle); }
  static HPCOMBI_CONSTEXPR
  Perm16 right_cycle() { return make_epu8(make_right_cycle); }
  static HPCOMBI_CONSTEXPR
  Perm16 left_shift() { return make_epu8(make_left_shift); }
  static HPCOMBI_CONSTEXPR
  Perm16 right_shift() { return make_epu8(make_right_shift); }
  static HPCOMBI_CONSTEXPR
  Perm16 left_shift_ff() { return make_epu8(make_left_shift_ff); }
  static HPCOMBI_CONSTEXPR
  Perm16 right_shift_ff() { return make_epu8(make_right_shift_ff); }


  inline static Perm16 elementary_transposition(uint64_t i);
  inline static Perm16 random();
  inline static Perm16 unrankSJT(int n, int r);

  inline Vect16 lehmer_ref() const;
  inline Vect16 lehmer() const;
  inline uint8_t length_ref() const;
  inline uint8_t length() const;

  inline uint8_t nb_descent_ref() const;
  inline uint8_t nb_descent() const;

  inline uint8_t nb_cycles_ref() const;
  inline Vect16 cycles_mask_unroll() const;
  inline uint8_t nb_cycles_unroll() const;
  inline uint8_t nb_cycles() const { return nb_cycles_unroll(); }

 private:
  inline static const std::array<Perm16, 3> inverting_rounds();
};

/*****************************************************************************/
/** Memory layout concepts check  ********************************************/
/*****************************************************************************/

static_assert(sizeof(Vect16) == sizeof(Perm16),
              "Vect16 and Perm16 have a different memory layout !");
static_assert(std::is_trivial<Vect16>(), "Vect16 is not a trivial class !");
static_assert(std::is_trivial<Perm16>(), "Perm16 is not a trivial class !");

const uint64_t prime = 0x9e3779b97f4a7bb9;
}  // namespace HPCombi

#include "perm16_impl.hpp"

namespace std {

template <> struct hash<HPCombi::Vect16> {
  inline size_t operator()(const HPCombi::Vect16 &ar) const {
    __int128 v0 = _mm_extract_epi64(ar.v, 0);
    __int128 v1 = _mm_extract_epi64(ar.v, 1);
    return ((v1 * HPCombi::prime + v0) * HPCombi::prime) >> 64;

    /* The following is extremely slow on Renner benchmark
    uint64_t v0 = _mm_extract_epi64(ar.v, 0);
    uint64_t v1 = _mm_extract_epi64(ar.v, 1);
    size_t seed = v0 + 0x9e3779b9;
    seed ^= v1 + 0x9e3779b9 + (seed<<6) + (seed>>2);
    return seed;
    */
  }
};

template <> struct hash<HPCombi::PTransf16> {
  inline size_t operator()(const HPCombi::PTransf16 &ar) const {
    return hash<HPCombi::Vect16>()(ar);
  }
};

template <> struct hash<HPCombi::Transf16> {
  inline size_t operator()(const HPCombi::Transf16 &ar) const {
    return uint64_t(ar);
  }
};

template <> struct hash<HPCombi::Perm16> {
  inline size_t operator()(const HPCombi::Perm16 &ar) const {
    return uint64_t(ar);
  }
};


template <> struct less<HPCombi::Vect16> {
  // WARNING: due to endianess this is not lexicographic comparison,
  //          but we don't care when using in std::set.
  // 10% faster than calling the lexicographic comparison operator !
  inline size_t operator()(const HPCombi::Vect16 &v1,
                           const HPCombi::Vect16 &v2) const {
    __m128 v1v = __m128(v1.v), v2v = __m128(v2.v);
    return v1v[0] == v2v[0] ? v1v[1] < v2v[1] : v1v[0] < v2v[0];
  }
};

}  // namespace std

#endif  // HPCOMBI_PERM16_HPP_INCLUDED
