#include <algorithm>
#include <chrono>
#include <cstdio>
#include <execution>
#include <future>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

#include <atomic>
#include <execution>
#include <future>
#include <thread>
#include <vector>

using std::milli;
using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;

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
// Implementation A: Multiple Promises and Atomic Boolean
// ===================================================================
template <typename Iterator, typename MatchType>
Iterator parallel_find_promise(Iterator first, Iterator last, MatchType match) {

  struct find_element {
    void operator()(Iterator begin, Iterator end, MatchType match,
                    std::promise<Iterator> *result, // promise to fill
                    std::atomic<bool> *done_flag    // atomic for early stop
    ) {
      try {
        // Actual Work
        // Check atomic on every cycle for early stop
        for (; (begin != end) && !std::atomic_load(done_flag); ++begin) {
          if (*begin == match) {
            result->set_value(begin);
            std::atomic_store(done_flag, true);
            return;
          }
        }
      } catch (...) {
        result->set_exception(std::current_exception());
        done_flag->store(true);
      }
    }
  };

  unsigned long const length = std::distance(first, last);

  if (!length) {
    return last;
  }

  // Calculate the optimized number of threads
  unsigned long const min_per_thread = 25;
  unsigned long const max_threads =
      (length + min_per_thread - 1) / min_per_thread;
  unsigned long const hardware_threads = std::thread::hardware_concurrency();
  unsigned long const num_threads =
      std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);
  unsigned long const block_size = length / num_threads;

  // Declare thread objects
  std::promise<Iterator> result;
  std::atomic<bool> done_flag(false);
  std::vector<std::thread> threads(num_threads - 1);

  // Split data into threads
  {
    join_threads joiner(threads);

    Iterator block_start = first;
    for (unsigned long i = 0; i < (num_threads - 1); i++) {
      Iterator block_end = block_start;
      std::advance(block_end, block_size);
      threads[i] = std::thread(find_element(), block_start, block_end, match,
                               &result, &done_flag);
      block_start = block_end;
    }

    // perform the find operation for final block in this thread.
    find_element()(block_start, last, match, &result, &done_flag);
  }

  // Threads are joined at this point!
  // Then, a done_flag==false, means not found.
  if (!done_flag.load()) {
    return last;
  }
  return result.get_future().get();
}

// ===================================================================
// Implementation B: Divide and Conquer Async
// ===================================================================
template <typename Iterator, typename MatchType>
Iterator parallel_find_async_impl(Iterator first, Iterator last,
                                  MatchType match,
                                  std::atomic<bool> *done_flag) {
  try {
    unsigned long const length = std::distance(first, last);
    unsigned long const min_per_thread = 25;

    printf("<parallel_find_async_impl> length: %lu\n", length);

    if (length < 2 * min_per_thread) {
      // Base Case
      for (; (first != last) && !std::atomic_load(done_flag); ++first) {
        if (*first == match) {
          std::atomic_store(done_flag, true);
          printf("<parallel_find_async_impl> found\n");
          return first;
        }
      }
      return last;

    } else {
      // Divide And Conquer: recurse and async
      Iterator const mid_point = first + length / 2;

      // async upper half
      std::future<Iterator> async_result =
          std::async(&parallel_find_async_impl<Iterator, MatchType>, mid_point,
                     last, match, std::ref(done_flag));

      // recurse lower half
      Iterator const direct_result =
          parallel_find_async_impl(first, mid_point, match, done_flag);

      return (direct_result == mid_point) ? async_result.get() : direct_result;
    }
  } catch (const std::exception &) {
    std::atomic_store(done_flag, true);
    throw;
  }
}

template <typename Iterator, typename MatchType>
Iterator parallel_find_async(Iterator first, Iterator last, MatchType match) {
  printf("parallel_find_async init\n");
  std::atomic<bool> done_flag{false};
  return parallel_find_async_impl(first, last, match, &done_flag);
}

// ===================================================================
// Benchmark
// ===================================================================

const size_t testSize = 1000;

using std::milli;
using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;

int main() {

  std::vector<int> ints(testSize);
  for (size_t i = 0; i < testSize; i++) {
    ints[i] = i;
  }

  int looking_for = 45;

  auto startTime = high_resolution_clock::now();
  parallel_find_promise(ints.begin(), ints.end(), looking_for);
  auto endTime = high_resolution_clock::now();
  print_results("Parallel-promise_atomic_impl :", startTime, endTime);

  startTime = high_resolution_clock::now();
  parallel_find_async(ints.begin(), ints.end(), looking_for);
  endTime = high_resolution_clock::now();
  print_results("Parallel-divide-and-conquer-async :", startTime, endTime);

  startTime = high_resolution_clock::now();
  std::find(ints.begin(), ints.end(), looking_for);
  endTime = high_resolution_clock::now();
  print_results("STL sequntial :", startTime, endTime);

  startTime = high_resolution_clock::now();
  std::find(std::execution::par, ints.begin(), ints.end(), looking_for);
  endTime = high_resolution_clock::now();
  print_results("STL parallel-par :", startTime, endTime);

  startTime = high_resolution_clock::now();
  std::find(std::execution::seq, ints.begin(), ints.end(), looking_for);
  endTime = high_resolution_clock::now();
  print_results("STL parallel-seq :", startTime, endTime);

  return 0;
}
