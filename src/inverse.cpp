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

#include <x86intrin.h>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <cstdlib>
#include <cstdint>
#include <array>
#include <vector>
#include <algorithm>

#include "perm16.hpp"
#include "testtools.hpp"

using namespace std;
using namespace std::chrono;
using namespace IVMPG;

int main() {
  std::srand(std::time(0));

  Perm16 p = {  5,  4, 12, 15, 10,  8,  9,  2,  3, 13, 14,  0,  1,  7, 11,  6};

  p = Perm16::random();

  cout << p << endl << p.inverse_ref() << endl;
  cout << p.inverse_pow() << endl;
  assert(p.inverse_ref() == p.inverse_sort());
  assert(p.inverse_ref() == p.inverse_find());
  assert(p.inverse_ref() == p.inverse_pow());
  assert(p * p.inverse_find() == Perm16::one());
  assert(p.inverse_find() * p == Perm16::one());
  assert(p * p.inverse_pow() == Perm16::one());
  assert(p.inverse_pow() * p == Perm16::one());

  cout << endl << endl;

  uint_fast64_t sz = 10000000;
  auto sample = rand_perms(sz);
  auto inv = sample;
  auto inv2 = sample;
  auto inv3 = sample;
  auto inv4 = sample;

  high_resolution_clock::time_point tstart, tfin;

  tstart = high_resolution_clock::now();
  for (uint_fast64_t i=0; i < sz; i++) inv[i] = sample[i].inverse_ref();
  tfin = high_resolution_clock::now();
  auto tmref = duration_cast<duration<double>>(tfin - tstart);
  cout << "timeref  = " << tmref.count() << "s" << endl;

  tstart = high_resolution_clock::now();
  for (uint_fast64_t i=0; i < sz; i++) inv2[i] = sample[i].inverse_sort();
  tfin = high_resolution_clock::now();
  auto tmsort = duration_cast<duration<double>>(tfin - tstart);
  cout << "timesort = " << tmsort.count() << "s";

  for (uint_fast64_t i=0; i < sz; i++) assert(inv[i] == inv2[i]);

  cout << ", speedup = " << tmref.count()/tmsort.count();
  cout << endl;

  tstart = high_resolution_clock::now();
  for (uint_fast64_t i=0; i < sz; i++) inv3[i] = sample[i].inverse_find();
  tfin = high_resolution_clock::now();
  auto tmnew = duration_cast<duration<double>>(tfin - tstart);
  cout << "timefind = " << tmnew.count() << "s";

  for (uint_fast64_t i=0; i < sz; i++) assert(inv[i] == inv3[i]);

  cout << ", speedup = " << tmref.count()/tmnew.count();
  cout << endl;

  tstart = high_resolution_clock::now();
  for (uint_fast64_t i=0; i < sz; i++) inv4[i] = sample[i].inverse_pow();
  tfin = high_resolution_clock::now();
  auto tmpow = duration_cast<duration<double>>(tfin - tstart);
  cout << "timepow  = " << tmpow.count() << "s";

  for (uint_fast64_t i=0; i < sz; i++) assert(inv[i] == inv4[i]);

  cout << ", speedup = " << tmref.count()/tmpow.count();
  cout << endl;

  return EXIT_SUCCESS;
}
