#include <algorithm>
#include <bits/c++config.h>
#include <chrono>
#include <execution>
#include <iostream>

#include <future>
#include <thread>
#include <vector>

using std::milli;
using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;

#define MIN_ELEMENTS_PER_THREAD 25

// ===================================================================
// Helper Elements
// ===================================================================
// Joins threads on destruction
class join_threads {
  std::vector<std::thread> &threads;

public:
  explicit join_threads(std::vector<std::thread> &_threads)
      : threads(_threads) {}

  ~join_threads() {
    for (std::size_t i = 0; i < threads.size(); i++) {
      if (threads[i].joinable())
        threads[i].join();
    }
  }
};

// Prints benchmark results
void print_results(const char *const tag,
                   high_resolution_clock::time_point startTime,
                   high_resolution_clock::time_point endTime) {
  printf("%s: Time: %fms\n", tag,
         duration_cast<duration<double, milli>>(endTime - startTime).count());
}

// ===================================================================
// Version 1: Using packaged_task and futures
// ===================================================================
template <typename Iterator, typename Func>
void parallel_for_each_pt(Iterator first, Iterator last, Func f) {
  unsigned long const length = std::distance(first, last);

  if (!length) {
    return;
  }

  // Calculate the optimized number of threads
  unsigned long const max_threads =
      (length + MIN_ELEMENTS_PER_THREAD - 1) / MIN_ELEMENTS_PER_THREAD;
  unsigned long const hardware_threads = std::thread::hardware_concurrency();
  unsigned long const num_threads =
      std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);
  unsigned long const block_size = length / num_threads;

  // Thread Objects
  std::vector<std::future<void>> futures(num_threads - 1);
  std::vector<std::thread> threads(num_threads - 1);
  join_threads joiner(threads);

  // Partition the data
  Iterator block_start = first;
  for (unsigned long i = 0; i <= (num_threads - 2); i++) {
    Iterator block_end = block_start;
    std::advance(block_end, block_size);

    // assign packaged_tasks to futures and threads.
    std::packaged_task<void(void)> task([=]() {
      // actual work happens here
      std::for_each(block_start, block_end, f);
    });
    futures[i] = task.get_future();
    threads[i] = std::thread(std::move(task));

    block_start = block_end;
  }

  // call the function for last block from this thread
  std::for_each(block_start, last, f);

  // wait
  for (unsigned long i = 0; i < (num_threads - 1); ++i) {
    futures[i].get();
  }
}

// ===================================================================
// Version 2: Using std::async
// ===================================================================
template <typename Iterator, typename Func>
void parallel_for_each_async(Iterator first, Iterator last, Func f) {
  unsigned long const length = std::distance(first, last);

  if (!length) {
    return;
  }

  if (length < 2 * MIN_ELEMENTS_PER_THREAD) {
    // base case
    std::for_each(first, last, f);
  } else {
    // divide and conquer
    Iterator const mid_point = first + length / 2;
    std::future<void> first_half = std::async(
        &parallel_for_each_async<Iterator, Func>, first, mid_point, f);

    parallel_for_each_async(mid_point, last, f);
    first_half.get();
  }
}

// ===================================================================
// Benchmark STL versions and own versions
// ===================================================================

const size_t testSize = 1000;

int main() {
  std::vector<int> ints(testSize);
  for (auto &i : ints) {
    i = 1;
  }

  auto long_function = [](const int &n) {
    int sum = 0;
    for (auto i = 0; i < 100000; i++) {
      sum += 1 * (i - 499);
    }
  };

  auto startTime = high_resolution_clock::now();
  std::for_each(ints.cbegin(), ints.cend(), long_function);
  auto endTime = high_resolution_clock::now();
  print_results("STL                   ", startTime, endTime);

  startTime = high_resolution_clock::now();
  for_each(std::execution::seq, ints.cbegin(), ints.cend(), long_function);
  endTime = high_resolution_clock::now();
  print_results("STL-seq               ", startTime, endTime);

  startTime = high_resolution_clock::now();
  std::for_each(std::execution::par, ints.cbegin(), ints.cend(), long_function);
  endTime = high_resolution_clock::now();
  print_results("STL-par               ", startTime, endTime);

  startTime = high_resolution_clock::now();
  parallel_for_each_pt(ints.cbegin(), ints.cend(), long_function);
  endTime = high_resolution_clock::now();
  print_results("Parallel-package_task ", startTime, endTime);

  startTime = high_resolution_clock::now();
  parallel_for_each_async(ints.cbegin(), ints.cend(), long_function);
  endTime = high_resolution_clock::now();
  print_results("Parallel-async        ", startTime, endTime);

  return 0;
}
