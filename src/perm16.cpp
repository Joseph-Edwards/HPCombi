//****************************************************************************//
//       Copyright (C) 2014 Florent Hivert <Florent.Hivert@lri.fr>,           //
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

#include "perm16.hpp"
#include <array>
#include <iostream>
#include <iomanip>
#include <algorithm>

namespace IVMPG {

// Definition since previously *only* declared
const constexpr size_t Vect16::Size;

// Sorting network Knuth AoCP3 Fig. 51 p 229.
const std::array<Vect16, 9> Vect16::sorting_rounds =
//     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15
  {{ { 1,  0,  3,  2,  5,  4,  7,  6,  9,  8, 11, 10, 13, 12, 15, 14},
     { 2,  3,  0,  1,  6,  7,  4,  5, 10, 11,  8,  9, 14, 15, 12, 13},
     { 4,  5,  6,  7,  0,  1,  2,  3, 12, 13, 14, 15,  8,  9, 10, 11},
     { 8,  9, 10, 11, 12, 13, 14, 15,  0,  1,  2,  3,  4,  5,  6,  7},
     { 0,  2,  1, 12,  8, 10,  9, 11,  4,  6,  5,  7,  3, 14, 13, 15},
     { 0,  4,  8, 10,  1,  9, 12, 13,  2,  5,  3, 14,  6,  7, 11, 15},
     { 0,  1,  4,  5,  2,  3,  8,  9,  6,  7, 12, 13, 10, 11, 14, 15},
     { 0,  1,  2,  6,  4,  8,  3, 10,  5, 12,  7, 11,  9, 13, 14, 15},
     { 0,  1,  2,  4,  3,  6,  5,  8,  7, 10,  9, 12, 11, 13, 14, 15}
    }};

const Perm16 Perm16::one =
  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

Perm16 Perm16::elementary_transposition(uint64_t i) {
  assert(i < vect::Size);
  Perm16 res {}; res[i] = i+1; res[i+1] = i; return res; }
Perm16 Perm16::random() {
  Perm16 res = Perm16::one;
  std::random_shuffle(res.p.begin(), res.p.end());
  return res;
}
// From Ruskey : Combinatorial Generation page 138
Perm16 Perm16::unrankSJT(int n, int r) {
  int j, k, rem, c;
  std::array<int, 16> dir;
  Perm16 res = Perm16::one;
  for (j=0; j < n; ++j) res[j] = 0xFF;
  for (j=n-1; j >= 0; --j) {
    rem = r % (j + 1);
    r = r / (j + 1);
    if ((r & 1) != 0) {
      k = -1; dir[j] = +1;
    } else {
      k = n; dir[j] = -1;
    }
    c = -1;
    do {
      k = k + dir[j];
      if (res[k] == 0xFF) ++c;
    } while (c < rem);
    res[k] = j;
  }
  return res;
}


std::ostream & operator<<(std::ostream & stream, Vect16 const &term) {
  stream << "[" << std::setw(2) << std::hex << unsigned(term[0]);
  for (unsigned i=1; i < Vect16::Size; ++i)
    stream << "," << std::setw(2) << std::hex << unsigned(term[i]);
  stream << "]";
  return stream;
}

}  // namespace IVMPG
