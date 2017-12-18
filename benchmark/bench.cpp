#include <benchmark/benchmark.h>
//~ #include "perm16.hpp"
//~ #include "perm_generic.hpp"
#include "bench_fixture.hpp"

using HPCombi::epu8;
using HPCombi::Vect16;
using HPCombi::PTransf16;
using HPCombi::Transf16;
using HPCombi::Perm16;
using HPCombi::VectGeneric;

static void escape(void *p) {
  asm volatile("" : : "g"(p) : "memory");
}

static void clobber() {
  asm volatile("" : : : "memory");
}

template<typename T>
void use(T &&t) {
  __asm__ __volatile__ ("" :: "g" (t));
}

const Fix_perm16 perm16_bench_data;
const Fix_generic generic_bench_data;

typedef Perm16 ( Perm16::*PERM16_OUT_FUNC ) () const;
typedef uint8_t ( Perm16::*UNINT8_OUT_FUNC ) () const;
typedef Vect1024 ( Vect1024::*COMPOSE_FUNC ) (const Vect1024&) const;
  
template<typename T, typename TF> 
void generic_register(benchmark::State& st, const char* label, const std::vector<T> sample, TF pfunc) { 
  for (auto _ : st) {
	  for (auto elem : sample){
		  benchmark::DoNotOptimize(
		  (elem.*pfunc)()
		  )
		  ;
		  escape(&elem.v);
	  }		  
	  clobber();
  }
  st.SetLabel(label);
}


  
void compose_register(benchmark::State& st, const char* label, const std::vector<Vect1024> sample, COMPOSE_FUNC compose_func) { 
  for (auto _ : st) {
	  for (auto elem : sample){
		  benchmark::DoNotOptimize(
		  (elem.*compose_func)(elem)
		  )
		  ;
		  escape(&elem.v);
	  }		  
	  clobber();
  }
  st.SetLabel(label);
}



int RegisterFromFunction_inverse() {
	//~ const float min_time = 0.00001;
    auto REF = benchmark::RegisterBenchmark("inverse_ref", &generic_register<Perm16, PERM16_OUT_FUNC>, "ref", perm16_bench_data.sample, &Perm16::inverse_ref);
    auto ALT_REF = benchmark::RegisterBenchmark("inverse_alt", &generic_register<Perm16, PERM16_OUT_FUNC>, "ref", perm16_bench_data.sample, &Perm16::inverse_ref);
    auto ALT_ARR = benchmark::RegisterBenchmark("inverse_alt", &generic_register<Perm16, PERM16_OUT_FUNC>, "arr", perm16_bench_data.sample, &Perm16::inverse_arr);
    auto ALT_SORT = benchmark::RegisterBenchmark("inverse_alt", &generic_register<Perm16, PERM16_OUT_FUNC>, "sort", perm16_bench_data.sample, &Perm16::inverse_sort);
    auto ALT_FIND = benchmark::RegisterBenchmark("inverse_alt", &generic_register<Perm16, PERM16_OUT_FUNC>, "find", perm16_bench_data.sample, &Perm16::inverse_find);
    auto ALT_POW = benchmark::RegisterBenchmark("inverse_alt", &generic_register<Perm16, PERM16_OUT_FUNC>, "pow", perm16_bench_data.sample, &Perm16::inverse_pow);
    auto ALT_CYCL = benchmark::RegisterBenchmark("inverse_alt", &generic_register<Perm16, PERM16_OUT_FUNC>, "cycl", perm16_bench_data.sample, &Perm16::inverse_cycl);	
    //~ const std::pair<std::string, const Vect16> vect16s[] = {{"inverse_ref", inverse}, {"inverse_arr", inverse}, {"inverse_sort", inverse}};
	//~ std::vector<benchmark::Benchmark> bnchs = {REF, ALT_REF, ALT_ARR, ALT_SORT, ALT_FIND, ALT_POW, ALT_CYCL};

	//~ REF->Unit(benchmark::kMillisecond); 
	
	//~ REF->MinTime(min_time); 
	//~ ALT_REF->MinTime(min_time); 
	//~ ALT_ARR->MinTime(min_time); 
	//~ ALT_SORT->MinTime(min_time); 
	//~ ALT_FIND->MinTime(min_time); 
	//~ ALT_POW->MinTime(min_time);
	//~ ALT_CYCL->MinTime(min_time);
}

int RegisterFromFunction_compose() {    
    auto REF_COMPOSE_CPU = benchmark::RegisterBenchmark("compose_ref", &compose_register, "ref", generic_bench_data.sample, &Vect1024::permuted);
    auto ALT_COMPOSE_CPU = benchmark::RegisterBenchmark("compose_alt", &compose_register, "cpu", generic_bench_data.sample, &Vect1024::permuted);
    #if COMPILE_CUDA==1
		auto ALT_COMPOSE_GPU = benchmark::RegisterBenchmark("compose_alt", &compose_register, "gpu", generic_bench_data.sample, &Vect1024::permuted_gpu);
    #endif  // USE_CUDA
  return 0;
}

int RegisterFromFunction() {
    auto REF_SUM = benchmark::RegisterBenchmark("sum_ref", &generic_register<Perm16, UNINT8_OUT_FUNC>, "ref", perm16_bench_data.sample, &Perm16::sum_ref);
    auto ALT_SUM_REF = benchmark::RegisterBenchmark("sum_alt", &generic_register<Perm16, UNINT8_OUT_FUNC>, "ref", perm16_bench_data.sample, &Perm16::sum_ref);
    auto ALT_SUM3 = benchmark::RegisterBenchmark("sum_alt", &generic_register<Perm16, UNINT8_OUT_FUNC>, "sum3", perm16_bench_data.sample, &Perm16::sum3);
    auto ALT_SUM4 = benchmark::RegisterBenchmark("sum_alt", &generic_register<Perm16, UNINT8_OUT_FUNC>, "sum4", perm16_bench_data.sample, &Perm16::sum4);

    auto REF_LENGTH = benchmark::RegisterBenchmark("length_ref", &generic_register<Perm16, UNINT8_OUT_FUNC>, "ref", perm16_bench_data.sample, &Perm16::length_ref);
    auto ALT_LENGTH_REF = benchmark::RegisterBenchmark("length_alt", &generic_register<Perm16, UNINT8_OUT_FUNC>, "ref", perm16_bench_data.sample, &Perm16::length_ref);
    auto ALT_LENGTH = benchmark::RegisterBenchmark("length_alt", &generic_register<Perm16, UNINT8_OUT_FUNC>, "length", perm16_bench_data.sample, &Perm16::length);

    auto REF_NB_DESCENT = benchmark::RegisterBenchmark("nb_descent_ref", &generic_register<Perm16, UNINT8_OUT_FUNC>, "ref", perm16_bench_data.sample, &Perm16::nb_descent_ref);
    auto ALT_NB_DESCENT_REF = benchmark::RegisterBenchmark("nb_descent_alt", &generic_register<Perm16, UNINT8_OUT_FUNC>, "ref", perm16_bench_data.sample, &Perm16::nb_descent_ref);
    auto ALT_NB_DESCENT = benchmark::RegisterBenchmark("nb_descent_alt", &generic_register<Perm16, UNINT8_OUT_FUNC>, "nb_descent", perm16_bench_data.sample, &Perm16::nb_descent);

    auto REF_NB_CYCLES = benchmark::RegisterBenchmark("nb_cycles_ref", &generic_register<Perm16, UNINT8_OUT_FUNC>, "ref", perm16_bench_data.sample, &Perm16::nb_cycles_ref);
    auto ALT_NB_CYCLES_REF = benchmark::RegisterBenchmark("nb_cycles_alt", &generic_register<Perm16, UNINT8_OUT_FUNC>, "ref", perm16_bench_data.sample, &Perm16::nb_cycles_ref);
    auto ALT_NB_CYCLES_UNROLL = benchmark::RegisterBenchmark("nb_cycles_alt", &generic_register<Perm16, UNINT8_OUT_FUNC>, "unroll", perm16_bench_data.sample, &Perm16::nb_cycles_unroll);
    
  return 0;
}

int dummy0 = RegisterFromFunction_compose();
int dummy2 = RegisterFromFunction_inverse();
int dummy1 = RegisterFromFunction();


BENCHMARK_MAIN();
