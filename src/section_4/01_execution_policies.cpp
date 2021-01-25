#include <algorithm>
#include <chrono>
#include <random>
#include <ratio>
#include <stddef.h>
#include <stdio.h>
#include <vector>

#include <execution>

const size_t testSize = 1'000'000;
const int iterationCount = 5;

void print_results(const char *const tag, const std::vector<double> &sorted,
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
  std::vector<double> doubles(testSize);
  for (auto &d : doubles) {
    d = static_cast<double>(rd());
  }

  // time how long it takes to sort them:
  for (int i = 0; i < iterationCount; ++i) {
    std::vector<double> sorted(doubles);
    const auto startTime = std::chrono::high_resolution_clock::now();
    std::sort(std::execution::seq, sorted.begin(), sorted.end());
    const auto endTime = std::chrono::high_resolution_clock::now();
    print_results("Serial STL ", sorted, startTime, endTime);
  }

  for (int i = 0; i < iterationCount; ++i) {
    std::vector<double> sorted(doubles);
    const auto startTime = std::chrono::high_resolution_clock::now();
    // same sort call as above, but with par_unseq:
    std::sort(std::execution::par, sorted.begin(), sorted.end());
    const auto endTime = std::chrono::high_resolution_clock::now();
    // in our output, note that these are the parallel results:
    print_results("Parallel STL", sorted, startTime, endTime);
  }
}
