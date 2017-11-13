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

#include <cstdlib>
#include <iostream>
#include "perm16.hpp"
#include "testtools.hpp"

using namespace std;
using namespace HPCombi;

int main() {
  auto vrand = rand_perms(1000);
  auto rep = 10000;

  cout << "Loop   : ";
  double reftime = timethat([vrand]() {
      for (Perm16 v : vrand) assert(v.sum_ref() == 120);
    }, rep);
  cout << "4 rnds : ";
  timethat([vrand]() {
      for (Perm16 v : vrand) assert(v.sum4() == 120);
    }, rep, reftime);
  cout << "3 rnds : ";
  timethat([vrand]() {
      for (Perm16 v : vrand) assert(v.sum3() == 120);
    }, rep, reftime);
  return EXIT_SUCCESS;
}
