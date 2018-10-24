//****************************************************************************//
//       Copyright (C) 2018 Florent Hivert <Florent.Hivert@lri.fr>,           //
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

#include <iostream>
#include <benchmark/benchmark.h>
#include <string.h>
#include <stdlib.h>

#include "compilerinfo.hpp"
#include "cpu_x86_impl.hpp"
#include "bench_fixture.hpp"

#include "perm16.hpp"
#include "perm_generic.hpp"

using namespace FeatureDetector;
using namespace std;
using HPCombi::epu8;

// const Fix_perm16 sample;
const Fix_epu8 sample;
const std::string SIMDSET = cpu_x86::get_highest_SIMD();
const std::string PROCID = cpu_x86::get_proc_string();

using HPCombi::epu8;
using HPCombi::Vect16;
using HPCombi::PTransf16;
using HPCombi::Transf16;
using HPCombi::Perm16;

//##################################################################################
template<typename TF, typename Sample>
void myBench(const string &name, TF pfunc, Sample &sample) {
    string fullname = name + "_" + CXX_VER + "_proc-" + PROCID;
    benchmark::RegisterBenchmark(fullname.c_str(),
        [pfunc](benchmark::State& st, Sample &sample) {
            for (auto _ : st) {
                for (auto elem : sample) {
                    benchmark::DoNotOptimize(pfunc(elem));
                }
            }
        }, sample);
}

#define myBenchLoop(nm, methmame, smp)  \
    myBench(nm, [](Perm16 p) { \
        for (int i = 0; i < 100; i++) p = p.methmame(); \
        return p; }, smp)
#define myBenchMeth(nm, methmame, smp) \
    myBench(nm, [](Perm16 p) { return p.methmame(); }, smp)


//##################################################################################
int Bench_inverse() {
    myBenchLoop("inverse_ref1", inverse_ref, sample.perms);
    myBenchLoop("inverse_ref2", inverse_ref, sample.perms);
    myBenchLoop("inverse_arr", inverse_arr, sample.perms);
    myBenchLoop("inverse_sort", inverse_sort, sample.perms);
    myBenchLoop("inverse_find", inverse_find, sample.perms);
    myBenchLoop("inverse_pow", inverse_pow, sample.perms);
    myBenchLoop("inverse_cycl", inverse_cycl, sample.perms);
    return 0;
}

int Bench_lehmer() {
    myBenchMeth("lehmer_ref1", lehmer_ref, sample.perms);
    myBenchMeth("lehmer_ref2", lehmer_ref, sample.perms);
    myBenchMeth("lehmer_arr", lehmer_arr, sample.perms);
    myBenchMeth("lehmer_opt", lehmer, sample.perms);
    return 0;
}

int Bench_length() {
    myBenchMeth("length_ref1", length_ref, sample.perms);
    myBenchMeth("length_ref2", length_ref, sample.perms);
    myBenchMeth("length_arr", length_arr, sample.perms);
    myBenchMeth("length_opt", length, sample.perms);
    return 0;
}

int Bench_nb_descents() {
    myBenchMeth("nb_descents_ref1", nb_descents_ref, sample.perms);
    myBenchMeth("nb_descents_ref2", nb_descents_ref, sample.perms);
    myBenchMeth("nb_descents_opt", nb_descents, sample.perms);
    return 0;
}

int Bench_nb_cycles() {
    myBenchMeth("nb_cycles_ref1", nb_cycles_ref, sample.perms);
    myBenchMeth("nb_cycles_ref2", nb_cycles_ref, sample.perms);
    myBenchMeth("nb_cycles_opt", nb_cycles, sample.perms);
    return 0;
}

auto dummy = {
    Bench_inverse(),
    Bench_lehmer(),
    Bench_length(),
    Bench_nb_descents(),
    Bench_nb_cycles()
};

BENCHMARK_MAIN();
