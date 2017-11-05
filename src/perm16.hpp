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

#ifndef PERM16_PERM16_HPP_INCLUDED
#define PERM16_PERM16_HPP_INCLUDED

#include <x86intrin.h>
#include <cassert>
#include <cstdint>
#include <array>
#include <ostream>
#include <functional>  // less<>

namespace IVMPG {

using epu8 = uint8_t __attribute__ ((vector_size (16)));


struct alignas(16) Vect16 {
  static const constexpr size_t Size = 16;
  epu8 v;

  Vect16() = default;
  constexpr Vect16(epu8 x) : v(x) {}
  Vect16(std::initializer_list<uint8_t> il);
  constexpr operator epu8() { return v; }
  constexpr operator const epu8() const { return v; }

  // Overload the default copy constructor and operator= : 10% speedup
  // But result in Non POD
  // constexpr Vect16(const Vect16 &x) : v(x.v) {}
  // Vect16 & operator=(const Vect16 &x) {v = x.v; return *this;}
  Vect16 & operator=(const epu8 &vv) {v = vv; return *this;}

  uint8_t operator[](uint64_t i) const { return v[i]; }
  uint8_t & operator[](uint64_t i) { return v[i]; }

  std::array<uint8_t, 16> &as_array() {
    return reinterpret_cast<std::array<unsigned char, 16>&>(v); }

  auto begin() { return as_array().begin(); }
  auto end() { return as_array().end(); }

  uint64_t first_diff(const Vect16 &b, size_t bound = Size) const;

  bool operator==(const Vect16 &b) const;
  bool operator!=(const Vect16 &b) const;
  bool operator<(const Vect16 &b) const;
  char less_partial(const Vect16 &b, int k) const;
  Vect16 permuted(const Vect16 &other) const;
  Vect16 sorted() const;
  Vect16 revsorted() const;

  uint8_t sum_ref() const;
  uint8_t sum4() const;
  uint8_t sum3() const;
  inline uint8_t sum() const { return sum3(); }

  template <char IDX_MODE> uint64_t search_index(int bound) const;

  uint64_t last_non_zero(int bnd = Size) const;
  uint64_t first_non_zero(int bnd = Size) const;
  uint64_t last_zero(int bnd = Size) const;
  uint64_t first_zero(int bnd = Size) const;

  Vect16 eval16_ref() const;
  Vect16 eval16_vect() const;

  bool is_permutation(const size_t k = Size) const;

  static Vect16 random(uint16_t bnd = 256);

 private:
  static const std::array<epu8, 9> sorting_rounds;
  static const std::array<epu8, 4> summing_rounds;
};


std::ostream & operator<<(std::ostream & stream, const Vect16 &term);

struct Perm16 : public Vect16 {
  using vect = Vect16;

  Perm16() = default;  // : Vect16({0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}) {};
  constexpr Perm16(const vect v) : vect(v) {}
  constexpr Perm16(const epu8 x) : vect(x) {}
  Perm16(std::initializer_list<uint8_t> il);

  Perm16 operator*(const Perm16&p) const { return permuted(p); }

  Perm16 inverse_ref() const;
  Perm16 inverse_sort() const;
  Perm16 inverse_find() const;
  Perm16 inverse_pow() const;
  Perm16 inverse_cycl() const;
  inline Perm16 inverse() { return inverse_cycl(); }

  // It's not possible to have a static constexpr member of same type as class
  // being defined (see https://stackoverflow.com/questions/11928089/)
  // therefore we chose to have functions.
  static const constexpr Perm16 one() {
    return epu8 {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  }
  static const constexpr Perm16 left_cycle() {
    return epu8 {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  }
  static const constexpr Perm16 right_cycle() {
    return epu8 {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0};
  }
  static const constexpr Perm16 left_shift_ff() {
    return epu8 {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0xff};
  }
  static const constexpr Perm16 left_shift() {
    return epu8 {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 15};
  }

  static Perm16 elementary_transposition(uint64_t i);
  static Perm16 random();
  static Perm16 unrankSJT(int n, int r);

  Vect16 lehmer_ref() const;
  Vect16 lehmer() const;
  uint8_t length_ref() const;
  uint8_t length() const;

  uint8_t nb_descent_ref() const;
  uint8_t nb_descent() const;

  uint8_t nb_cycles_ref() const;
  Vect16 cycles_mask_unroll() const;
  uint8_t nb_cycles_unroll() const;
  inline uint8_t nb_cycles() const { return nb_cycles_unroll(); }

 private:
  static const std::array<Perm16, 3> inverting_rounds;
};

}  // namespace IVMPG

#include "perm16_impl.hpp"

namespace std {

#define MASK_24 (((u_int32_t)1<<24)-1) /* i.e., (u_int32_t)0xffffff */

template<>
struct hash<IVMPG::Vect16> {
  inline size_t operator () (const IVMPG::Vect16 &ar) const {
    __int128 v0 = _mm_extract_epi64(ar.v, 0);
    __int128 v1 = _mm_extract_epi64(ar.v, 1);
    return ((v1*IVMPG::prime + v0)*IVMPG::prime) >> 64;
  }
};

template<>
struct less<IVMPG::Vect16> {
  // WARNING: due to endianess this is not lexicographic comparison,
  //          but we don't care when using in std::set.
  // 10% faster than calling the lexicographic comparison operator !
  inline size_t operator() (const IVMPG::Vect16 &v1,
                            const IVMPG::Vect16 &v2) const {
    __m128 v1v = __m128(v1.v), v2v = __m128(v2.v);
    return v1v[0] == v2v[0] ? v1v[1] < v2v[1] : v1v[0] < v2v[0];
  }
};

}  // namespace std

#endif   // PERM16_PERM16_HPP_INCLUDED
