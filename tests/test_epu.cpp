/******************************************************************************/
/*     Copyright (C) 2016-2018 Florent Hivert <Florent.Hivert@lri.fr>,        */
/*                                                                            */
/*  Distributed under the terms of the GNU General Public License (GPL)       */
/*                                                                            */
/*    This code is distributed in the hope that it will be useful,            */
/*    but WITHOUT ANY WARRANTY; without even the implied warranty of          */
/*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       */
/*   General Public License for more details.                                 */
/*                                                                            */
/*  The full text of the GPL is available at:                                 */
/*                                                                            */
/*                  http://www.gnu.org/licenses/                              */
/******************************************************************************/

#include <iostream>
#include <vector>

#include "epu.hpp"

#include "test_main.hpp"
#include <catch2/catch_test_macros.hpp>

// TODO uncomment
// #define TEST_AGREES(ref, fun) \
//     BOOST_FIXTURE_TEST_CASE(Epu8::agrees_##fun, Fix) { \
//         for (auto x : v) \
//             REQUIRE(fun(x) == ref(x)); \
//     }

namespace HPCombi {

struct Fix {
    Fix()
        : zero(Epu8({}, 0)), P01(Epu8({0, 1}, 0)), P10(Epu8({1, 0}, 0)),
          P11(Epu8({1, 1}, 0)), P1(Epu8({}, 1)), P112(Epu8({1, 1}, 2)),
          Pa(epu8{1, 2, 3, 4, 0, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}),
          Pb(epu8{1, 2, 3, 6, 0, 5, 4, 7, 8, 9, 10, 11, 12, 15, 14, 13}),
          RP(epu8{3, 1, 0, 14, 15, 13, 5, 10, 2, 11, 6, 12, 7, 4, 8, 9}),
          Pa1(Epu8({4, 2, 5, 1, 2, 7, 7, 3, 4, 2}, 1)),
          Pa2(Epu8({4, 2, 5, 1, 2, 9, 7, 3, 4, 2}, 1)), P51(Epu8({5, 1}, 6)),
          Pv(epu8{5, 5, 2, 5, 1, 6, 12, 4, 0, 3, 2, 11, 12, 13, 14, 15}),
          Pw(epu8{5, 5, 2, 9, 1, 6, 12, 4, 0, 4, 4, 4, 12, 13, 14, 15}),
          P5(Epu8({}, 5)), Pc(Epu8({23, 5, 21, 5, 43, 36}, 7)),
          // Elements should be sorted in alphabetic order here
          v({zero, P01, epu8id, P10, P11, P1, P112, Pa, Pb, RP, Pa1, Pa2, P51,
             Pv, Pw, P5, epu8rev, Pc}),
          av({{5, 5, 2, 5, 1, 6, 12, 4, 0, 3, 2, 11, 12, 13, 14, 15}}) {}
    ~Fix() = default;

    const epu8 zero, P01, P10, P11, P1, P112, Pa, Pb, RP, Pa1, Pa2, P51, Pv, Pw,
        P5, Pc;
    const std::vector<epu8> v;
    const std::array<uint8_t, 16> av;
};

TEST_CASE_METHOD(Fix, "Epu8::first_diff_ref", "[Epu8][000]") {
    REQUIRE(first_diff_ref(Pc, Pc) == 16);
    REQUIRE(first_diff_ref(zero, P01) == 1);
    REQUIRE(first_diff_ref(zero, P10) == 0);
    REQUIRE(first_diff_ref(zero, P01, 1) == 16);
    REQUIRE(first_diff_ref(zero, P01, 2) == 1);
    REQUIRE(first_diff_ref(Pa1, Pa2, 2) == 16);
    REQUIRE(first_diff_ref(Pa1, Pa2, 4) == 16);
    REQUIRE(first_diff_ref(Pa1, Pa2, 5) == 16);
    REQUIRE(first_diff_ref(Pa1, Pa2, 6) == 5);
    REQUIRE(first_diff_ref(Pa1, Pa2, 7) == 5);
    REQUIRE(first_diff_ref(Pa1, Pa2) == 5);
    REQUIRE(first_diff(Pv, Pw) == 3);
    for (int i = 0; i < 16; i++)
        REQUIRE(first_diff(Pv, Pw, i) == (i <= 3 ? 16 : 3));
}

#ifdef SIMDE_X86_SSE4_2_NATIVE
TEST_CASE_METHOD(Fix, "Epu8::first_diff_cmpstr", "[Epu8][001]") {
    for (auto x : v) {
        for (auto y : v) {
            REQUIRE(first_diff_cmpstr(x, y) == first_diff_ref(x, y));
            for (int i = 0; i < 17; i++)
                REQUIRE(first_diff_cmpstr(x, y, i) == first_diff_ref(x, y, i));
        }
    }
}
#endif
TEST_CASE_METHOD(Fix, "Epu8::first_diff_mask", "[Epu8][002]") {
    for (auto x : v) {
        for (auto y : v) {
            REQUIRE(first_diff_mask(x, y) == first_diff_ref(x, y));
            for (int i = 0; i < 17; i++)
                REQUIRE(first_diff_mask(x, y, i) == first_diff_ref(x, y, i));
        }
    }
}

TEST_CASE_METHOD(Fix, "Epu8::last_diff_ref", "[Epu8][003]") {
    REQUIRE(last_diff_ref(Pc, Pc) == 16);
    REQUIRE(last_diff_ref(zero, P01) == 1);
    REQUIRE(last_diff_ref(zero, P10) == 0);
    REQUIRE(last_diff_ref(zero, P01, 1) == 16);
    REQUIRE(last_diff_ref(zero, P01, 2) == 1);
    REQUIRE(last_diff_ref(P1, Pa1) == 9);
    REQUIRE(last_diff_ref(P1, Pa1, 12) == 9);
    REQUIRE(last_diff_ref(P1, Pa1, 9) == 8);
    REQUIRE(last_diff_ref(Pa1, Pa2, 2) == 16);
    REQUIRE(last_diff_ref(Pa1, Pa2, 4) == 16);
    REQUIRE(last_diff_ref(Pa1, Pa2, 5) == 16);
    REQUIRE(last_diff_ref(Pa1, Pa2, 6) == 5);
    REQUIRE(last_diff_ref(Pa1, Pa2, 7) == 5);
    REQUIRE(last_diff_ref(Pa1, Pa2) == 5);
    const std::array<uint8_t, 17> res{
        {16, 16, 16, 16, 3, 3, 3, 3, 3, 3, 9, 10, 11, 11, 11, 11, 11}};
    for (int i = 0; i <= 16; i++) {
        REQUIRE(last_diff_ref(Pv, Pw, i) == res[i]);
    }
}
#ifdef SIMDE_X86_SSE4_2_NATIVE
TEST_CASE_METHOD(Fix, "Epu8::last_diff_cmpstr", "[Epu8][004]") {
    for (auto x : v) {
        for (auto y : v) {
            REQUIRE(last_diff_cmpstr(x, y) == last_diff_ref(x, y));
            for (int i = 0; i < 17; i++)
                REQUIRE(last_diff_cmpstr(x, y, i) == last_diff_ref(x, y, i));
        }
    }
}
#endif

TEST_CASE_METHOD(Fix, "Epu8::last_diff_mask", "[Epu8][005]") {
    for (auto x : v) {
        for (auto y : v) {
            REQUIRE(last_diff_mask(x, y) == last_diff_ref(x, y));
            for (int i = 0; i < 17; i++)
                REQUIRE(last_diff_mask(x, y, i) == last_diff_ref(x, y, i));
        }
    }
}

TEST_CASE_METHOD(Fix, "Epu8::is_all_zero", "[Epu8][006]") {
    REQUIRE(is_all_zero(zero));
    for (size_t i = 1; i < v.size(); i++) {
        REQUIRE(!is_all_zero(v[i]));
    }
}

TEST_CASE_METHOD(Fix, "Epu8::is_all_one", "[Epu8][007]") {
    for (size_t i = 0; i < v.size(); i++) {
        REQUIRE(!is_all_one(v[i]));
    }
    REQUIRE(is_all_one(Epu8(0xFF)));
}

TEST_CASE_METHOD(Fix, "Epu8::equal", "[Epu8][008]") {
    for (size_t i = 0; i < v.size(); i++) {
        epu8 a = v[i];
        for (size_t j = 0; j < v.size(); j++) {
            epu8 b = v[j];
            if (i == j) {
                REQUIRE(equal(a, b));
                REQUIRE(!not_equal(a, b));
                REQUIRE(std::equal_to<epu8>()(a, b));
                REQUIRE(!std::not_equal_to<epu8>()(a, b));
            } else {
                REQUIRE(!equal(a, b));
                REQUIRE(not_equal(a, b));
                REQUIRE(std::not_equal_to<epu8>()(a, b));
                REQUIRE(!std::equal_to<epu8>()(a, b));
            }
        }
    }
}

TEST_CASE_METHOD(Fix, "Epu8::not_equal", "[Epu8][009]") {
    for (size_t i = 0; i < v.size(); i++) {
        for (size_t j = 0; j < v.size(); j++) {
            if (i == j) {
                REQUIRE(!not_equal(v[i], v[j]));
            } else {
                REQUIRE(not_equal(v[i], v[j]));
            }
        }
    }
}

TEST_CASE_METHOD(Fix, "Epu8::less", "[Epu8][010]") {
    for (size_t i = 0; i < v.size(); i++) {
        for (size_t j = 0; j < v.size(); j++) {
            if (i < j) {
                REQUIRE(less(v[i], v[j]));
            } else {
                REQUIRE(!less(v[i], v[j]));
            }
        }
    }
}

TEST_CASE_METHOD(Fix, "Epu8::permuted", "[Epu8][011]") {
    REQUIRE_THAT(
        permuted(epu8{0, 1, 3, 2, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                 epu8{3, 2, 5, 1, 4, 0, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}),
        Equals(epu8{2, 3, 5, 1, 4, 0, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}));
    REQUIRE_THAT(
        permuted(epu8{3, 2, 5, 1, 4, 0, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                 epu8{0, 1, 3, 2, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}),
        Equals(epu8{3, 2, 1, 5, 4, 0, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}));
    REQUIRE_THAT(
        permuted(epu8{3, 2, 5, 1, 4, 0, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                 epu8{2, 2, 1, 2, 3, 6, 12, 4, 5, 16, 17, 11, 12, 13, 14, 15}),
        Equals(epu8{5, 5, 2, 5, 1, 6, 12, 4, 0, 3, 2, 11, 12, 13, 14, 15}));
}

TEST_CASE_METHOD(Fix, "Epu8::shifted_left", "[Epu8][012]") {
    REQUIRE_THAT(shifted_left(P01), Equals(P10));
    REQUIRE_THAT(shifted_left(P112),
                 Equals(epu8{1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0}));
    REQUIRE_THAT(shifted_left(Pv), Equals(epu8{5, 2, 5, 1, 6, 12, 4, 0, 3, 2,
                                               11, 12, 13, 14, 15, 0}));
}

TEST_CASE_METHOD(Fix, "Epu8::shifted_right", "[Epu8][013]") {
    REQUIRE_THAT(shifted_right(P10), Equals(P01));
    REQUIRE_THAT(shifted_right(P112), Equals(Epu8({0, 1, 1}, 2)));
    REQUIRE_THAT(shifted_right(Pv), Equals(epu8{0, 5, 5, 2, 5, 1, 6, 12, 4, 0,
                                                3, 2, 11, 12, 13, 14}));
}

TEST_CASE_METHOD(Fix, "Epu8::reverted", "[Epu8][014]") {
    REQUIRE_THAT(reverted(epu8id), Equals(epu8rev));
    for (auto x : v) {
        REQUIRE_THAT(x, Equals(reverted(reverted(x))));
    }
}

TEST_CASE_METHOD(Fix, "Epu8::as_array", "[Epu8][015]") {
    epu8 x = Epu8({4, 2, 5, 1, 2, 7, 7, 3, 4, 2}, 1);
    auto &refx = as_array(x);
    refx[2] = 42;
    REQUIRE_THAT(x, Equals(Epu8({4, 2, 42, 1, 2, 7, 7, 3, 4, 2}, 1)));
    std::fill(refx.begin() + 4, refx.end(), 3);
    REQUIRE_THAT(x, Equals(Epu8({4, 2, 42, 1}, 3)));
    REQUIRE(av == as_array(Pv));
}

TEST_CASE_METHOD(Fix, "Epu8::from_array", "[Epu8][016]") {
    for (auto x : v) {
        REQUIRE_THAT(x, Equals(from_array(as_array(x))));
    }
    REQUIRE_THAT(Pv, Equals(from_array(av)));
}

TEST_CASE_METHOD(Fix, "Epu8::is_sorted", "[Epu8][017]") {
    REQUIRE(is_sorted(epu8id));
    REQUIRE(
        is_sorted(epu8{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}));
    REQUIRE(is_sorted(Epu8({0, 1}, 2)));
    REQUIRE(is_sorted(Epu8({0}, 1)));
    REQUIRE(is_sorted(Epu8({}, 5)));
    REQUIRE(
        !is_sorted(epu8{0, 1, 3, 2, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}));
    REQUIRE(!is_sorted(Epu8({0, 2}, 1)));
    REQUIRE(!is_sorted(Epu8({0, 0, 2}, 1)));
    REQUIRE(!is_sorted(Epu8({6}, 5)));

    epu8 x = epu8id;
    REQUIRE(is_sorted(x));
    auto &refx = as_array(x);
    while (std::next_permutation(refx.begin(), refx.begin() + 9)) {
        REQUIRE(!is_sorted(x));
    }
    x = epu8id;
    while (std::next_permutation(refx.begin() + 8, refx.begin() + 16)) {
        REQUIRE(!is_sorted(x));
    }
    x = sorted(Pa1);
    REQUIRE(is_sorted(x));
    while (std::next_permutation(refx.begin(), refx.begin() + 14)) {
        REQUIRE(!is_sorted(x));
    }
}

TEST_CASE_METHOD(Fix, "Epu8::sorted", "[Epu8][018]") {
    REQUIRE_THAT(
        sorted(epu8{0, 1, 3, 2, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}),
        Equals(epu8id));
    for (auto &x : v) {
        REQUIRE(is_sorted(sorted(x)));
    }
    epu8 x = epu8id;
    REQUIRE(is_sorted(x));
    auto &refx = as_array(x);
    do {
        REQUIRE(is_sorted(sorted(x)));
    } while (std::next_permutation(refx.begin(), refx.begin() + 9));
}

TEST_CASE_METHOD(Fix, "Epu8::revsorted", "[Epu8][019]") {
    REQUIRE_THAT(
        revsorted(epu8{0, 1, 3, 2, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}),
        Equals(epu8rev));
    for (auto &x : v) {
        REQUIRE(is_sorted(reverted(revsorted(x))));
    }
    epu8 x = epu8id;
    REQUIRE(is_sorted(x));
    auto &refx = as_array(x);
    do {
        REQUIRE(is_sorted(reverted(revsorted(x))));
    } while (std::next_permutation(refx.begin(), refx.begin() + 9));
}

TEST_CASE_METHOD(Fix, "Epu8::sort_perm", "[Epu8][020]") {
    epu8 ve{2, 1, 3, 2, 4, 1, 1, 4, 2, 0, 1, 2, 1, 3, 4, 0};
    REQUIRE_THAT(sort_perm(ve),
                  Equals(epu8{9, 15, 1, 5, 6, 10, 12, 3, 0, 8, 11, 2, 13, 7, 4, 14}));
    REQUIRE_THAT(ve, Equals(epu8{0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 4, 4, 4}));

    for (auto x : v) {
        epu8 xsort = x;
        epu8 psort = sort_perm(xsort);
        REQUIRE(is_sorted(xsort));
        REQUIRE(is_permutation(psort));
        REQUIRE_THAT(permuted(x, psort), Equals(xsort));
    }
}

TEST_CASE_METHOD(Fix, "Epu8::sort8_perm", "[Epu8][021]") {
    epu8 ve{2, 1, 3, 2, 4, 1, 1, 4, 2, 0, 1, 2, 1, 3, 4, 0};
    REQUIRE_THAT(
        sort8_perm(ve),
        Equals(epu8{1, 6, 5, 0, 3, 2, 4, 7, 9, 15, 10, 12, 8, 11, 13, 14}));
    REQUIRE_THAT(ve,
                 Equals(epu8{1, 1, 1, 2, 2, 3, 4, 4, 0, 0, 1, 1, 2, 2, 3, 4}));

    for (auto x : v) {
        epu8 xsort = x;
        epu8 psort = sort_perm(xsort);
        REQUIRE(is_sorted(xsort | Epu8({0, 0, 0, 0, 0, 0, 0, 0}, 0xFF)));
        REQUIRE(is_sorted(xsort & Epu8({0, 0, 0, 0, 0, 0, 0, 0}, 0xFF)));
        REQUIRE(is_permutation(psort));
        REQUIRE_THAT(permuted(x, psort), Equals(xsort));
    }
}

TEST_CASE_METHOD(Fix, "Epu8::permutation_of", "[Epu8][022]") {
    REQUIRE_THAT(permutation_of(epu8id, epu8id), Equals(epu8id));
    REQUIRE_THAT(permutation_of(Pa, Pa), Equals(epu8id));
    REQUIRE_THAT(permutation_of(epu8rev, epu8id), Equals(epu8rev));
    REQUIRE_THAT(permutation_of(epu8id, epu8rev), Equals(epu8rev));
    REQUIRE_THAT(permutation_of(epu8rev, epu8rev), Equals(epu8id));
    REQUIRE_THAT(permutation_of(epu8id, RP), Equals(RP));
    const uint8_t FF = 0xff;
    REQUIRE_THAT(
        (permutation_of(Pv, Pv) |
         epu8{FF, FF, FF, FF, 0, 0, FF, 0, 0, 0, FF, 0, FF, 0, 0, 0}),
        Equals(epu8{FF, FF, FF, FF, 4, 5, FF, 7, 8, 9, FF, 11, FF, 13, 14, 15}));
}
TEST_CASE_METHOD(Fix, "Epu8::permutation_of_ref", "[Epu8][022]") {
    REQUIRE_THAT(permutation_of_ref(epu8id, epu8id), Equals(epu8id));
    REQUIRE_THAT(permutation_of_ref(Pa, Pa), Equals(epu8id));
    REQUIRE_THAT(permutation_of_ref(epu8rev, epu8id), Equals(epu8rev));
    REQUIRE_THAT(permutation_of_ref(epu8id, epu8rev), Equals(epu8rev));
    REQUIRE_THAT(permutation_of_ref(epu8rev, epu8rev), Equals(epu8id));
    REQUIRE_THAT(permutation_of_ref(epu8id, RP), Equals(RP));
    const uint8_t FF = 0xff;
    REQUIRE_THAT(
        (permutation_of_ref(Pv, Pv) |
         epu8{FF, FF, FF, FF, 0, 0, FF, 0, 0, 0, FF, 0, FF, 0, 0, 0}),
        Equals(epu8{FF, FF, FF, FF, 4, 5, FF, 7, 8, 9, FF, 11, FF, 13, 14, 15}));
}

TEST_CASE_METHOD(Fix, "Epu8::remove_dups", "[Epu8][023]") {
    REQUIRE_THAT(remove_dups(P1), Equals(P10));
    REQUIRE_THAT(remove_dups(P11), Equals(P10));
    REQUIRE_THAT(remove_dups(sorted(P10)),
                 Equals(epu8{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}));
    REQUIRE_THAT(
        remove_dups(sorted(Pv)),
        Equals(epu8{0, 1, 2, 0, 3, 4, 5, 0, 0, 6, 11, 12, 0, 13, 14, 15}));
    REQUIRE_THAT(remove_dups(P1, 1), Equals(P1));
    REQUIRE_THAT(remove_dups(P11, 1), Equals(Epu8({1, 1, 0}, 1)));
    REQUIRE_THAT(remove_dups(P11, 42), Equals(Epu8({1, 42, 0}, 42)));
    REQUIRE_THAT(remove_dups(sorted(P10), 1), Equals(P1));
    REQUIRE_THAT(
        remove_dups(sorted(Pv), 7),
        Equals(epu8{7, 1, 2, 7, 3, 4, 5, 7, 7, 6, 11, 12, 7, 13, 14, 15}));
    for (auto x : v) {
        x = sorted(remove_dups(sorted(x)));
        REQUIRE_THAT(x, Equals(sorted(remove_dups(x))));
    }
    for (auto x : v) {
        x = sorted(remove_dups(sorted(x), 42));
        REQUIRE_THAT(x, Equals(sorted(remove_dups(x, 42))));
    }
}

TEST_CASE_METHOD(Fix, "Epu8::horiz_sum_ref", "[Epu8][024]") {
    REQUIRE(horiz_sum_ref(zero) == 0);
    REQUIRE(horiz_sum_ref(P01) == 1);
    REQUIRE(horiz_sum_ref(epu8id) == 120);
    REQUIRE(horiz_sum_ref(P10) == 1);
    REQUIRE(horiz_sum_ref(P11) == 2);
    REQUIRE(horiz_sum_ref(P1) == 16);
    REQUIRE(horiz_sum_ref(P112) == 30);
    REQUIRE(horiz_sum_ref(Pa1) == 43);
    REQUIRE(horiz_sum_ref(Pa2) == 45);
    REQUIRE(horiz_sum_ref(P51) == 90);
    REQUIRE(horiz_sum_ref(Pv) == 110);
    REQUIRE(horiz_sum_ref(P5) == 80);
    REQUIRE(horiz_sum_ref(epu8rev) == 120);
    REQUIRE(horiz_sum_ref(Pc) == 203);
}

// TODO uncomment, compiler complains that Epu8 is an unknown type!?
// TEST_AGREES(Fix, Epu8, horiz_sum_ref, horiz_sum_gen, v, "[Epu8][025]")
// TEST_AGREES(Fix, Epu8, horiz_sum_ref, horiz_sum4, v, "[Epu8][026]")
//
// TEST_AGREES(Fix, Epu8, horiz_sum_ref, horiz_sum3, v, "[Epu8][027]")
// TEST_AGREES(Fix, Epu8, horiz_sum_ref, horiz_sum, v, "[Epu8][028]")

TEST_CASE_METHOD(Fix, "Epu8::partial_sums_ref", "[Epu8][029]") {
    REQUIRE_THAT(partial_sums_ref(zero), Equals(zero));
    REQUIRE_THAT(partial_sums_ref(P01), Equals(Epu8({0}, 1)));
    REQUIRE_THAT(partial_sums_ref(epu8id),
                 Equals(epu8{0, 1, 3, 6, 10, 15, 21, 28, 36, 45, 55, 66, 78, 91,
                             105, 120}));
    REQUIRE_THAT(partial_sums_ref(P10), Equals(P1));
    REQUIRE_THAT(partial_sums_ref(P11), Equals(Epu8({1}, 2)));
    REQUIRE_THAT(partial_sums_ref(P1), Equals(epu8id + Epu8({}, 1)));
    REQUIRE_THAT(partial_sums_ref(P112),
                 Equals(epu8{1, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26,
                             28, 30}));
    REQUIRE_THAT(partial_sums_ref(Pa1),
                 Equals(epu8{4, 6, 11, 12, 14, 21, 28, 31, 35, 37, 38, 39, 40,
                             41, 42, 43}));

    REQUIRE_THAT(partial_sums_ref(Pa2),
                 Equals(epu8{4, 6, 11, 12, 14, 23, 30, 33, 37, 39, 40, 41, 42,
                             43, 44, 45}));
    REQUIRE_THAT(partial_sums_ref(P51),
                 Equals(epu8{5, 6, 12, 18, 24, 30, 36, 42, 48, 54, 60, 66, 72,
                             78, 84, 90}));
    REQUIRE_THAT(partial_sums_ref(Pv),
                 Equals(epu8{5, 10, 12, 17, 18, 24, 36, 40, 40, 43, 45, 56, 68,
                             81, 95, 110}));
    REQUIRE_THAT(partial_sums_ref(P5),
                 Equals(epu8{5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65,
                             70, 75, 80}));
    REQUIRE_THAT(partial_sums_ref(epu8rev),
                 Equals(epu8{15, 29, 42, 54, 65, 75, 84, 92, 99, 105, 110, 114,
                             117, 119, 120, 120}));
    REQUIRE_THAT(partial_sums_ref(Pc),
                 Equals(epu8{23, 28, 49, 54, 97, 133, 140, 147, 154, 161, 168,
                             175, 182, 189, 196, 203}));
}
TEST_CASE_METHOD(Fix, "Epu8::partial_sum_gen", "[Epu8][030]") {
    for (auto x : v) {
        REQUIRE_THAT(partial_sums_gen(x), Equals(partial_sums_ref(x)));
    }
}
TEST_CASE_METHOD(Fix, "Epu8::partial_sum_round", "[Epu8][031]") {
    for (auto x : v) {
        REQUIRE_THAT(partial_sums_round(x), Equals(partial_sums_ref(x)));
    }
}
TEST_CASE_METHOD(Fix, "Epu8::partial_sum", "[Epu8][032]") {
    for (auto x : v) {
        REQUIRE_THAT(partial_sums(x), Equals(partial_sums_ref(x)));
    }
}

TEST_CASE_METHOD(Fix, "Epu8::horiz_max_ref", "[Epu8][033]") {
    REQUIRE(horiz_max_ref(zero) == 0);
    REQUIRE(horiz_max_ref(P01) == 1);
    REQUIRE(horiz_max_ref(epu8id) == 15);
    REQUIRE(horiz_max_ref(P10) == 1);
    REQUIRE(horiz_max_ref(P11) == 1);
    REQUIRE(horiz_max_ref(P1) == 1);
    REQUIRE(horiz_max_ref(P112) == 2);
    REQUIRE(horiz_max_ref(Pa1) == 7);
    REQUIRE(horiz_max_ref(Pa2) == 9);
    REQUIRE(horiz_max_ref(P51) == 6);
    REQUIRE(horiz_max_ref(Pv) == 15);
    REQUIRE(horiz_max_ref(P5) == 5);
    REQUIRE(horiz_max_ref(epu8rev) == 15);
    REQUIRE(horiz_max_ref(Pc) == 43);
}

// TODO uncomment, compiler complains that Epu8 is an unknown type!?
// TEST_AGREES(Fix, Epu8, horiz_max_ref, horiz_max_gen, v, "[Epu8][034]")
// TEST_AGREES(Fix, Epu8, horiz_max_ref, horiz_max4, v, "[Epu8][035]")
// TEST_AGREES(Fix, Epu8, horiz_max_ref, horiz_max3, v, "[Epu8][036]")
// TEST_AGREES(Fix, Epu8, horiz_max_ref, horiz_max, v, "[Epu8][037]")

TEST_CASE_METHOD(Fix, "Epu8::partial_max_ref", "[Epu8][038]") {
    REQUIRE_THAT(partial_max_ref(zero), Equals(zero));
    REQUIRE_THAT(partial_max_ref(P01), Equals(Epu8({0}, 1)));
    REQUIRE_THAT(partial_max_ref(epu8id), Equals(epu8id));
    REQUIRE_THAT(partial_max_ref(P10), Equals(P1));
    REQUIRE_THAT(partial_max_ref(P11), Equals(P1));
    REQUIRE_THAT(partial_max_ref(P1), Equals(P1));
    REQUIRE_THAT(partial_max_ref(P112), Equals(P112));
    REQUIRE_THAT(partial_max_ref(Pa1), Equals(Epu8({4, 4, 5, 5, 5}, 7)));
    REQUIRE_THAT(partial_max_ref(Pa2), Equals(Epu8({4, 4, 5, 5, 5}, 9)));
    REQUIRE_THAT(partial_max_ref(P51), Equals(Epu8({5, 5}, 6)));
    REQUIRE_THAT(
        partial_max_ref(Pv),
        Equals(epu8{5, 5, 5, 5, 5, 6, 12, 12, 12, 12, 12, 12, 12, 13, 14, 15}));
    REQUIRE_THAT(partial_max_ref(P5), Equals(P5));
    REQUIRE_THAT(partial_max_ref(epu8rev), Equals(Epu8({}, 15)));
    REQUIRE_THAT(partial_max_ref(Pc), Equals(Epu8({23, 23, 23, 23}, 43)));
}

TEST_CASE_METHOD(Fix, "Epu8::partial_max_gen", "[Epu8][039]") {
    for (auto x : v) {
        REQUIRE_THAT(partial_max_gen(x), Equals(partial_max_ref(x)));
    }
}

TEST_CASE_METHOD(Fix, "Epu8::partial_max_round", "[Epu8][040]") {
    for (auto x : v) {
        REQUIRE_THAT(partial_max_round(x), Equals(partial_max_ref(x)));
    }
}

TEST_CASE_METHOD(Fix, "Epu8::partial_max", "[Epu8][041]") {
    for (auto x : v) {
        REQUIRE_THAT(partial_max(x), Equals(partial_max_ref(x)));
    }
}

TEST_CASE_METHOD(Fix, "Epu8::horiz_min_ref", "[Epu8][042]") {
    REQUIRE(horiz_min_ref(zero) == 0);
    REQUIRE(horiz_min_ref(P01) == 0);
    REQUIRE(horiz_min_ref(epu8id) == 0);
    REQUIRE(horiz_min_ref(P10) == 0);
    REQUIRE(horiz_min_ref(P11) == 0);
    REQUIRE(horiz_min_ref(P1) == 1);
    REQUIRE(horiz_min_ref(P112) == 1);
    REQUIRE(horiz_min_ref(Pa1) == 1);
    REQUIRE(horiz_min_ref(Pa2) == 1);
    REQUIRE(horiz_min_ref(P51) == 1);
    REQUIRE(horiz_min_ref(Pv) == 0);
    REQUIRE(horiz_min_ref(P5) == 5);
    REQUIRE(horiz_min_ref(epu8rev) == 0);
    REQUIRE(horiz_min_ref(Pc) == 5);
}
// TODO uncomment
// TEST_AGREES(horiz_min_ref, horiz_min_gen)
// TEST_AGREES(horiz_min_ref, horiz_min4)
// TEST_AGREES(horiz_min_ref, horiz_min3)
// TEST_AGREES(horiz_min_ref, horiz_min)

TEST_CASE_METHOD(Fix, "Epu8::partial_min_ref", "[Epu8][043]") {
    REQUIRE_THAT(partial_min_ref(zero), Equals(zero));
    REQUIRE_THAT(partial_min_ref(P01), Equals(zero));
    REQUIRE_THAT(partial_min_ref(epu8id), Equals(zero));
    REQUIRE_THAT(partial_min_ref(P10), Equals(P10));
    REQUIRE_THAT(partial_min_ref(P11), Equals(P11));
    REQUIRE_THAT(partial_min_ref(P1), Equals(P1));
    REQUIRE_THAT(partial_min_ref(P112), Equals(P1));
    REQUIRE_THAT(partial_min_ref(Pa1), Equals(Epu8({4, 2, 2}, 1)));
    REQUIRE_THAT(partial_min_ref(Pa2), Equals(Epu8({4, 2, 2}, 1)));
    REQUIRE_THAT(partial_min_ref(P51), Equals(Epu8({5}, 1)));
    REQUIRE_THAT(
        partial_min_ref(Pv),
        Equals(Epu8({5, 5, 2, 2, 1, 1, 1, 1, }, 0)));
    REQUIRE_THAT(partial_min_ref(P5), Equals(P5));
    REQUIRE_THAT(partial_min_ref(epu8rev), Equals(epu8rev));
    REQUIRE_THAT(partial_min_ref(Pc), Equals(Epu8({23}, 5)));
}

TEST_CASE_METHOD(Fix, "Epu8::partial_min_gen", "[Epu8][044]") {
    for (auto x : v) {
        REQUIRE_THAT(partial_min_gen(x), Equals(partial_min_ref(x)));
    }
}

TEST_CASE_METHOD(Fix, "Epu8::partial_min_round", "[Epu8][045]") {
    for (auto x : v) {
        REQUIRE_THAT(partial_min_round(x), Equals(partial_min_ref(x)));
    }
}

TEST_CASE_METHOD(Fix, "Epu8::partial_min", "[Epu8][046]") {
    for (auto x : v) {
        REQUIRE_THAT(partial_min(x), Equals(partial_min_ref(x)));
    }
}

TEST_CASE_METHOD(Fix, "Epu8::eval16_ref", "[Epu8][047]") {
    REQUIRE_THAT(eval16_ref(zero), Equals(Epu8({16}, 0)));
    REQUIRE_THAT(eval16_ref(P01), Equals(Epu8({15, 1}, 0)));
    REQUIRE_THAT(eval16_ref(epu8id), Equals(Epu8({}, 1)));
    REQUIRE_THAT(eval16_ref(P10), Equals(Epu8({15, 1}, 0)));
    REQUIRE_THAT(eval16_ref(P11), Equals(Epu8({14, 2}, 0)));
    REQUIRE_THAT(eval16_ref(P1), Equals(Epu8({0, 16}, 0)));
    REQUIRE_THAT(eval16_ref(P112), Equals(Epu8({0, 2, 14}, 0)));
    REQUIRE_THAT(eval16_ref(Pa1), Equals(Epu8({0, 7, 3, 1, 2, 1, 0, 2}, 0)));
    REQUIRE_THAT(eval16_ref(Pa2),
                 Equals(Epu8({0, 7, 3, 1, 2, 1, 0, 1, 0, 1}, 0)));
    REQUIRE_THAT(eval16_ref(P51), Equals(Epu8({0, 1, 0, 0, 0, 1, 14}, 0)));
    REQUIRE_THAT(eval16_ref(Pv),
                 Equals(epu8{1, 1, 2, 1, 1, 3, 1, 0, 0, 0, 0, 1, 2, 1, 1, 1}));
    REQUIRE_THAT(eval16_ref(P5), Equals(Epu8({0, 0, 0, 0, 0, 16}, 0)));
    REQUIRE_THAT(eval16_ref(epu8rev), Equals(Epu8({}, 1)));
    REQUIRE_THAT(eval16_ref(Pc), Equals(Epu8({0, 0, 0, 0, 0, 2, 0, 10}, 0)));
}
// TODO uncomment
// TEST_Epu8::AGREES(eval16_ref, eval16_cycle, "[Epu8][000]")
// TEST_Epu8::AGREES(eval16_ref, eval16_popcount, "[Epu8][000]")
// TEST_Epu8::AGREES(eval16_ref, eval16_arr, "[Epu8][000]")
// TEST_Epu8::AGREES(eval16_ref, eval16_gen, "[Epu8][000]")
// TEST_Epu8::AGREES(eval16_ref, eval16, "[Epu8][000]")

TEST_CASE("Epu8::popcount4", "[Epu8][048]") {
    REQUIRE_THAT(popcount4,
                 Equals(epu8{0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4}));
}

TEST_CASE_METHOD(Fix, "Epu8::popcount16", "[Epu8][049]") {
    REQUIRE_THAT(popcount16(Pv),
                 Equals(epu8{2, 2, 1, 2, 1, 2, 2, 1, 0, 2, 1, 3, 2, 3, 3, 4}));
    REQUIRE_THAT(popcount16(RP),
                 Equals(epu8{2, 1, 0, 3, 4, 3, 2, 2, 1, 3, 2, 2, 3, 1, 1, 2}));
    REQUIRE_THAT(popcount16(RP << 1),
                 Equals(epu8{2, 1, 0, 3, 4, 3, 2, 2, 1, 3, 2, 2, 3, 1, 1, 2}));
    REQUIRE_THAT(popcount16(RP << 2),
                 Equals(epu8{2, 1, 0, 3, 4, 3, 2, 2, 1, 3, 2, 2, 3, 1, 1, 2}));
    REQUIRE_THAT(popcount16(Epu8({0, 1, 5, 0xff, 0xf0, 0x35}, 0x0f)),
                 Equals(Epu8({0, 1, 2, 8}, 4)));
}

TEST_CASE("random_epu8", "[Epu8][050]") {
    for (int i = 0; i < 10; i++) {
        epu8 r = random_epu8(255);
        REQUIRE_THAT(r, Equals(r));
    }
}

TEST_CASE_METHOD(Fix, "is_partial_transformation", "[Epu8][051]") {
    REQUIRE(is_partial_transformation(zero));
    REQUIRE(is_partial_transformation(P01));
    REQUIRE(is_partial_transformation(P10));
    REQUIRE(!is_partial_transformation(Epu8({16}, 0)));
    REQUIRE(is_partial_transformation(Epu8({}, 0xff)));
    REQUIRE(is_partial_transformation(Epu8({2, 0xff, 3}, 0)));

    REQUIRE(!is_partial_transformation(zero, 15));
    REQUIRE(is_partial_transformation(Pa));
    REQUIRE(is_partial_transformation(Pa, 6));
    REQUIRE(is_partial_transformation(Pa, 5));
    REQUIRE(!is_partial_transformation(Pa, 4));
    REQUIRE(!is_partial_transformation(Pa, 1));
    REQUIRE(!is_partial_transformation(Pa, 0));

    REQUIRE(is_partial_transformation(RP));
    REQUIRE(is_partial_transformation(RP, 16));
    REQUIRE(!is_partial_transformation(RP, 15));
    REQUIRE(is_partial_transformation(Epu8({1, 2, 1, 0xFF, 0, 5, 0xFF, 2}, 0)));
    REQUIRE(
        !is_partial_transformation(Epu8({1, 2, 1, 0xFF, 0, 16, 0xFF, 2}, 0)));
}

TEST_CASE_METHOD(Fix, "is_transformation", "[Epu8][052]") {
    REQUIRE(is_transformation(zero));
    REQUIRE(is_transformation(P01));
    REQUIRE(is_transformation(P10));
    REQUIRE(!is_transformation(Epu8({16}, 0)));
    REQUIRE(!is_transformation(Epu8({}, 0xff)));
    REQUIRE(!is_transformation(Epu8({2, 0xff, 3}, 0)));

    REQUIRE(!is_transformation(zero, 15));
    REQUIRE(is_transformation(Pa));
    REQUIRE(is_transformation(Pa, 6));
    REQUIRE(is_transformation(Pa, 5));
    REQUIRE(!is_transformation(Pa, 4));
    REQUIRE(!is_transformation(Pa, 1));
    REQUIRE(!is_transformation(Pa, 0));

    REQUIRE(is_transformation(RP));
    REQUIRE(is_transformation(RP, 16));
    REQUIRE(!is_transformation(RP, 15));
}

TEST_CASE_METHOD(Fix, "is_partial_permutation", "[Epu8][053]") {
    REQUIRE(!is_partial_permutation(zero));
    REQUIRE(!is_partial_permutation(P01));
    REQUIRE(!is_partial_permutation(P10));
    REQUIRE(!is_partial_permutation(Epu8({16}, 0)));
    REQUIRE(is_partial_permutation(Epu8({}, 0xff)));
    REQUIRE(!is_partial_permutation(Epu8({2, 0xff, 3}, 0)));
    REQUIRE(is_partial_permutation(Epu8({2, 0xff, 3}, 0xff)));

    REQUIRE(!is_partial_permutation(zero, 15));
    REQUIRE(is_partial_permutation(Pa));
    REQUIRE(is_partial_permutation(Pa, 6));
    REQUIRE(is_partial_permutation(Pa, 5));
    REQUIRE(!is_partial_permutation(Pa, 4));
    REQUIRE(!is_partial_permutation(Pa, 1));
    REQUIRE(!is_partial_permutation(Pa, 0));

    REQUIRE(is_partial_permutation(RP));
    REQUIRE(is_partial_permutation(RP, 16));
    REQUIRE(!is_partial_permutation(RP, 15));

    REQUIRE(is_partial_permutation(
        epu8{1, 2, 0xFF, 0xFF, 0, 5, 0xFF, 3, 8, 9, 10, 11, 12, 13, 14, 15}));
    REQUIRE(!is_partial_permutation(
        epu8{1, 2, 1, 0xFF, 0, 5, 0xFF, 2, 8, 9, 10, 11, 12, 13, 14, 15}));
    REQUIRE(!is_partial_permutation(Epu8({1, 2, 1, 0xFF, 0, 5, 0xFF, 2}, 0)));
    REQUIRE(!is_partial_permutation(Epu8({1, 2, 1, 0xFF, 0, 16, 0xFF, 2}, 0)));
}

TEST_CASE_METHOD(Fix, "is_permutation", "[Epu8][054]") {
    REQUIRE(!is_permutation(zero));
    REQUIRE(!is_permutation(P01));
    REQUIRE(!is_permutation(P10));
    REQUIRE(!is_permutation(Epu8({16}, 0)));
    REQUIRE(!is_permutation(Epu8({}, 0xff)));
    REQUIRE(!is_permutation(Epu8({2, 0xff, 3}, 0)));

    REQUIRE(!is_permutation(zero, 15));
    REQUIRE(is_permutation(Pa));
    REQUIRE(is_permutation(Pa, 6));
    REQUIRE(is_permutation(Pa, 5));
    REQUIRE(!is_permutation(Pa, 4));
    REQUIRE(!is_permutation(Pa, 1));
    REQUIRE(!is_permutation(Pa, 0));

    REQUIRE(is_permutation(RP));
    REQUIRE(is_permutation(RP, 16));
    REQUIRE(!is_permutation(RP, 15));
}

#ifdef SIMDE_X86_SSE4_2_NATIVE
TEST_CASE_METHOD(Fix, "is_permutation_cmpestri", "[Epu8][070]") {
    for (auto x : v) {
        for (size_t i = 0; i < 16; i++) {
            REQUIRE(is_permutation(x, i) == is_permutation_cmpestri(x, i));
        }
    }
}
#endif

TEST_CASE_METHOD(Fix, "is_permutation_sort", "[Epu8][080]") {
    for (auto x : v) {
        for (size_t i = 0; i < 16; i++) {
            REQUIRE(is_permutation(x, i) == is_permutation_sort(x, i));
        }
    }
}

}  // namespace HPCombi
