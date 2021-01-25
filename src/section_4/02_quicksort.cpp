#include <algorithm>
#include <chrono>
#include <future>
#include <list>
#include <random>
#include <ratio>
#include <stddef.h>
#include <stdio.h>
#include <vector>

#include <execution>

const size_t testSize = 100'000;
const int iterationCount = 5;

template <typename T> std::list<T> parallel_quick_sort(std::list<T> input) {

  // Base Case
  if (input.size() < 2) {
    return input;
  }

  // select pivot
  std::list<T> result;
  result.splice(result.begin(), input, input.begin());
  T pivot = *result.begin();

  // partition the data
  auto divide_point = std::partition(input.begin(), input.end(),
                                     [&](T const &t) { return t < pivot; });
  std::list<T> lower_list;
  lower_list.splice(lower_list.end(), input, input.begin(), divide_point);

  // =================================================================
  // Example on how to parallelize divide and conquer algorithms.
  // =================================================================
  // recursion on the lower part, threading on the upper part.
  auto new_lower(parallel_quick_sort(std::move(lower_list)));
  std::future<std::list<T>> new_upper_future(
      std::async(&parallel_quick_sort<T>, std::move(input)));

  // return
  result.splice(result.begin(), new_lower);
  result.splice(result.end(), new_upper_future.get());
  // =================================================================

  return result;
}

void print_results(const char *const tag, const std::list<double> &sorted,
                   std::chrono::high_resolution_clock::time_point startTime,
                   std::chrono::high_resolution_clock::time_point endTime) {
  printf("%s: Lowest: %g Highest: %g Time: %fms\n", tag, sorted.front(),
         sorted.back(),
         std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(
             endTime - startTime)
             .count());
}

int main() {

  // generate some random doubles:
  printf("Testing with %zu doubles...\n", testSize);
  std::random_device rd;
  std::list<double> doubles(testSize);
  for (auto &d : doubles) {
    d = static_cast<double>(rd());
  }

  for (int i = 0; i < iterationCount; ++i) {
    std::list<double> sorted(doubles);
    const auto startTime = std::chrono::high_resolution_clock::now();
    // same sort call as above, but with par_unseq:
    parallel_quick_sort(doubles);
    const auto endTime = std::chrono::high_resolution_clock::now();
    // in our output, note that these are the parallel results:
    print_results("Parallel STL", sorted, startTime, endTime);
  }
}
