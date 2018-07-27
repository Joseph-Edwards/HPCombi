/******************************************************************************/
/*       Copyright (C) 2017 Florent Hivert <Florent.Hivert@lri.fr>,           */
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

#define BOOST_TEST_MODULE EPUTests

#include <boost/test/unit_test.hpp>
#include <boost/functional.hpp>
#include <vector>

#include "epu.hpp"
#include <iostream>

using namespace HPCombi;

#define EPU8_EQUAL(p1, p2)  BOOST_CHECK_PREDICATE(equal, (p1)(p2))
#define EPU8_NOT_EQUAL(p1, p2)  BOOST_CHECK_PREDICATE(boost::not2(equal), (p1)(p2))


struct Fix {
    Fix() : zero(Epu8({}, 0)), P01(Epu8({0, 1}, 0)),
            P10(Epu8({1, 0}, 0)), P11(Epu8({1, 1}, 0)),
            P1(Epu8({}, 1)),
            P112(Epu8({1, 1}, 2)),
            Pa1(Epu8({4, 2, 5, 1, 2, 7, 7, 3, 4, 2}, 1)),
            Pa2(Epu8({4, 2, 5, 1, 2, 9, 7, 3, 4, 2}, 1)),
            P51(Epu8({5,1}, 6)),
            Pv(epu8{ 5, 5, 2, 5, 1, 6,12, 4, 0, 3, 2,11,12,13,14,15}),
            P5(Epu8({}, 5)),
            Pb(Epu8({23, 5, 21, 5, 43, 36}, 7)),
            // Elements should be sorted in alphabetic order here
            v({zero, P01, P10, P11, P1, P112, Pa1, Pa2, P51, Pv, P5, Pb}),
            av({ 5, 5, 2, 5, 1, 6,12, 4, 0, 3, 2,11,12,13,14,15})
        {
            BOOST_TEST_MESSAGE("setup fixture");
        }
    ~Fix() { BOOST_TEST_MESSAGE("teardown fixture"); }

    const epu8 zero, P01, P10, P11, P1, P112, Pa1, Pa2, P51, Pv, P5, Pb;
    const std::vector<epu8> v;
    const std::array<uint8_t, 16> av;
};

//****************************************************************************//
BOOST_AUTO_TEST_SUITE(EPU8_test)
//****************************************************************************//

BOOST_FIXTURE_TEST_CASE(EPU8_first_diff, Fix) {
    BOOST_TEST(first_diff(Pb, Pb) == 16);
    BOOST_TEST(first_diff(zero, P01) == 1);
    BOOST_TEST(first_diff(zero, P10) == 0);
    BOOST_TEST(first_diff(zero, P01, 1) == 16);
    BOOST_TEST(first_diff(zero, P01, 2) == 1);
    BOOST_TEST(first_diff(Pa1, Pa2, 2) == 16);
    BOOST_TEST(first_diff(Pa1, Pa2, 4) == 16);
    BOOST_TEST(first_diff(Pa1, Pa2, 5) == 16);
    BOOST_TEST(first_diff(Pa1, Pa2, 6) == 5);
    BOOST_TEST(first_diff(Pa1, Pa2, 7) == 5);
    BOOST_TEST(first_diff(Pa1, Pa2) == 5);
}

BOOST_FIXTURE_TEST_CASE(EPU8_last_diff, Fix) {
    BOOST_TEST(last_diff(Pb, Pb) == 16);
    BOOST_TEST(last_diff(zero, P01) == 1);
    BOOST_TEST(last_diff(zero, P10) == 0);
    BOOST_TEST(last_diff(zero, P01, 1) == 16);
    BOOST_TEST(last_diff(zero, P01, 2) == 1);
    BOOST_TEST(last_diff(P1, Pa1) == 9);
    BOOST_TEST(last_diff(P1, Pa1, 12) == 9);
    BOOST_TEST(last_diff(P1, Pa1, 9) == 8);
    BOOST_TEST(last_diff(Pa1, Pa2, 2) == 16);
    BOOST_TEST(last_diff(Pa1, Pa2, 4) == 16);
    BOOST_TEST(last_diff(Pa1, Pa2, 5) == 16);
    BOOST_TEST(last_diff(Pa1, Pa2, 6) == 5);
    BOOST_TEST(last_diff(Pa1, Pa2, 7) == 5);
    BOOST_TEST(last_diff(Pa1, Pa2) == 5);
}

BOOST_FIXTURE_TEST_CASE(EPU8_is_all_zero, Fix) {
    BOOST_TEST(is_all_zero(Fix::zero));
    for (size_t i = 1; i < Fix::v.size(); i++) {
        BOOST_TEST(not is_all_zero(Fix::v[i]));
    }
}
BOOST_FIXTURE_TEST_CASE(EPU8_equal, Fix) {
    for (size_t i = 0; i < Fix::v.size(); i++) {
        epu8 a = Fix::v[i];
        for (size_t j = 0; j < Fix::v.size(); j++) {
            epu8 b = Fix::v[j];
            if (i == j) {
                BOOST_CHECK_PREDICATE(equal, (a)(b));
                BOOST_CHECK_PREDICATE(boost::not2(not_equal), (a)(b));
                BOOST_CHECK_PREDICATE(std::equal_to<epu8>(), (a)(b));
//  For some reason, the following line doesn't compile
//              BOOST_CHECK_PREDICATE(boost::not2(std::not_equal_to<epu8>()),
//                                    (a)(b));
                BOOST_CHECK_PREDICATE(
                    [](epu8 a, epu8 b) {
                        return not std::not_equal_to<epu8>()(a, b);
                    }, (a)(b));
            } else {
                BOOST_CHECK_PREDICATE(boost::not2(equal), (a)(b));
                BOOST_CHECK_PREDICATE(not_equal, (a)(b));
                BOOST_CHECK_PREDICATE(std::not_equal_to<epu8>(), (a)(b));
//  For some reason, the following line doesn't compile
//              BOOST_CHECK_PREDICATE(boost::not2(std::equal_to<epu8>()), (a)(b));
                BOOST_CHECK_PREDICATE(
                    [](epu8 a, epu8 b) {
                        return not std::equal_to<epu8>()(a, b);
                    }, (a)(b));
            }
        }
    }
}

BOOST_FIXTURE_TEST_CASE(EPU8_not_equal, Fix) {
    for (size_t i = 0; i < Fix::v.size(); i++)
        for (size_t j = 0; j < Fix::v.size(); j++)
            if (i == j)
                BOOST_CHECK_PREDICATE(boost::not2(not_equal),
                                      (Fix::v[i])(Fix::v[j]));
            else
                BOOST_CHECK_PREDICATE(not_equal, (Fix::v[i])(Fix::v[j]));
}

BOOST_FIXTURE_TEST_CASE(EPU8_less, Fix) {
    for (size_t i = 0; i < Fix::v.size(); i++)
        for (size_t j = 0; j < Fix::v.size(); j++)
            if (i < j)
                BOOST_CHECK_PREDICATE(less, (Fix::v[i])(Fix::v[j]));
            else
                BOOST_CHECK_PREDICATE(boost::not2(less), (Fix::v[i])(Fix::v[j]));
}

BOOST_FIXTURE_TEST_CASE(EPU8_permuted, Fix) {
    EPU8_EQUAL(permuted(epu8{ 0, 1, 3, 2, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15},
                        epu8{ 3, 2, 5, 1, 4, 0, 6, 7, 8, 9,10,11,12,13,14,15}),
               (epu8        { 2, 3, 5, 1, 4, 0, 6, 7, 8, 9,10,11,12,13,14,15}));
    EPU8_EQUAL(permuted(epu8{ 3, 2, 5, 1, 4, 0, 6, 7, 8, 9,10,11,12,13,14,15},
                        epu8{ 0, 1, 3, 2, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15}),
               (epu8        { 3, 2, 1, 5, 4, 0, 6, 7, 8, 9,10,11,12,13,14,15}));
    EPU8_EQUAL(permuted(epu8{ 3, 2, 5, 1, 4, 0, 6, 7, 8, 9,10,11,12,13,14,15},
                        epu8{ 2, 2, 1, 2, 3, 6,12, 4, 5,16,17,11,12,13,14,15}),
               (epu8        { 5, 5, 2, 5, 1, 6,12, 4, 0, 3, 2,11,12,13,14,15}));
}

BOOST_FIXTURE_TEST_CASE(EPU8_as_array, Fix) {
    epu8 x = Epu8({4, 2, 5, 1, 2, 7, 7, 3, 4, 2}, 1);
    auto & refx = as_array(x);
    refx[2] = 42;
    EPU8_EQUAL(x, Epu8({4, 2, 42, 1, 2, 7, 7, 3, 4, 2}, 1));
    std::fill(refx.begin()+4, refx.end(), 3);
    EPU8_EQUAL(x, Epu8({4, 2, 42, 1}, 3));
    BOOST_TEST(av == as_array(Pv));
}

BOOST_FIXTURE_TEST_CASE(EPU8_from_array, Fix) {
    for (auto x : v) {
        EPU8_EQUAL(x, from_array(as_array(x)));
    }
    EPU8_EQUAL(Pv, from_array(av));
}

BOOST_FIXTURE_TEST_CASE(EPU8_is_sorted, Fix) {
    BOOST_TEST(is_sorted(epu8id));
    BOOST_TEST(is_sorted(epu8{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15}));
    BOOST_TEST(is_sorted(Epu8({ 0, 1}, 2)));
    BOOST_TEST(is_sorted(Epu8({0}, 1)));
    BOOST_TEST(is_sorted(Epu8({}, 5)));
    BOOST_TEST(not is_sorted(epu8{ 0, 1, 3, 2, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15}));
    BOOST_TEST(not is_sorted(Epu8({ 0, 2}, 1)));
    BOOST_TEST(not is_sorted(Epu8({ 0, 0, 2}, 1)));
    BOOST_TEST(not is_sorted(Epu8({6}, 5)));

    epu8 x = epu8id;
    BOOST_TEST(is_sorted(x));
    auto & refx = as_array(x);
    while (std::next_permutation(refx.begin(), refx.begin()+9)) {
        BOOST_TEST(not is_sorted(x));
    }
    x = epu8id;
    while (std::next_permutation(refx.begin()+8, refx.begin()+16)) {
        BOOST_TEST(not is_sorted(x));
    }
    x = sorted(Pa1);
    BOOST_TEST(is_sorted(x));
    while (std::next_permutation(refx.begin(), refx.begin()+14)) {
        BOOST_TEST(not is_sorted(x));
    }
}

BOOST_FIXTURE_TEST_CASE(EPU8_sorted, Fix) {
    EPU8_EQUAL(sorted(epu8{ 0, 1, 3, 2, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15}),
               epu8id);
    for (auto &x : Fix::v)
        BOOST_TEST(is_sorted(sorted(x)));
    epu8 x = epu8id;
    BOOST_TEST(is_sorted(x));
    auto & refx = as_array(x);
    do {
        BOOST_TEST(is_sorted(sorted(x)));
    } while (std::next_permutation(refx.begin(), refx.begin()+9));
}


BOOST_AUTO_TEST_SUITE_END()
//****************************************************************************//
