#include <benchmark/benchmark.h>

#include <random>

#include "calc/calc.h"

template <typename Q> static void BM_RandomStrings(benchmark::State &state) {
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<char> char_dist('a', 'z');

    int N = state.range(0);

    for (auto _ : state) {
        state.PauseTiming();
        std::vector<std::string> input;
        for (int i = 0; i < N; ++i) {
            std::string str;
            for (int j = 0; j < 10; ++j) {
                str += char_dist(rng);
            }
            input.push_back(str);
        }
        state.ResumeTiming();
        auto ret = Q::dedupe(input);
        benchmark::DoNotOptimize(ret);
    }
    state.SetComplexityN(N);
}

#define REGISTER_BENCHMARK(FuncType) \
    BENCHMARK_TEMPLATE(BM_RandomStrings, FuncType) \
        ->RangeMultiplier(2) \
        ->Range(1 << 4, 1 << 12) \
        ->Complexity() \
        ->Unit(benchmark::kMillisecond);

REGISTER_BENCHMARK(UseSet);
REGISTER_BENCHMARK(UseRuntimeConfigurableSet);

BENCHMARK_MAIN();