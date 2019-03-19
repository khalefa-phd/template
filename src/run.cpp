#include "profile.hpp"
#include "tbb/tbb.h"
#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <random>
#include <vector>

using namespace std;
static void escape(void* p) { asm volatile("" : : "g"(p) : "memory"); }

bool writeHeader = true;

int main() {
   PerfEvents e;
   size_t n = 1000 * 1000 * 10;
   e.timeAndProfile("one thread", n,
                    [&]() {
                       vector<int> v;
                       for (size_t i = 0; i < n; i++) { v.push_back(i); }
                       escape(&v);
                    },
                    1);

   e.timeAndProfile("ten thread", n,
                    [&]() {
                       tbb::enumerable_thread_specific<vector<int>> v;
                       tbb::parallel_for(

                           tbb::blocked_range<size_t>(0, n, n / 10),
                           [&](const tbb::blocked_range<size_t>& r) {
                              auto& vv = v.local();
                              for (size_t i = r.begin(), end = r.end();
                                   i != end; ++i)
                                 vv.push_back(i);
                           });
                       escape(&v);
                    },
                    1);

   return 0;
}
