#include <algorithm>
#include <cstdio>
#include <functional>
#include <iostream>
#include <iterator>
#include <numeric>
#include <string>
#include <thread>
#include <vector>

/**
Sequencial accumulate API:
https://en.cppreference.com/w/cpp/algorithm/accumulate

  template < class InputIt, class T>
  constexpr T accumulate(InputIt first, InputIt last, T init );

  template < class InputIt, class T, class BinaryOperation>
  constexpr T accumulate(InputIt first, InputIt last, T init,
                         BinaryOperation op );
*/
void sequential_accumulate_test() {
  std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  int sum = std::accumulate(v.begin(), v.end(), 0);
  int prod = std::accumulate(v.begin(), v.end(), 1, std::multiplies<int>());
  printf("sum: %d\n", sum);
  printf("product: %d\n", prod);

  auto dash_fold = [](std::string a, int b) {
    return std::move(a) + "-" + std::to_string(b);
  };
  std::string s = std::accumulate(std::next(v.begin()), v.end(),
                                  std::to_string(v[0]), dash_fold);
  std::cout << "dash fold - " << s << std::endl;
}

#define MIN_BLOCK_SIZE 1000

// Accumulate Wrapper using reference to the data
template <class InputIt, class T>
void accumulate(InputIt first, InputIt last, T init, T &ref) {
  ref = std::accumulate(first, last, init);
}

// Parallel Accumulate uses reference to data for return
template <class InputIt, class T>
T parallel_accumulate(InputIt first, InputIt last, T init) {

  //  m_threads and block_size
  int input_size = std::distance(first, last);
  int allowed_threads_by_elements = (input_size) / MIN_BLOCK_SIZE;
  int allowed_threads_by_hardware = std::thread::hardware_concurrency();
  int num_threads =
      std::min(allowed_threads_by_hardware, allowed_threads_by_elements);
  int block_size = (input_size + 1) / num_threads;

  printf("\n");
  printf("Parallel Accumulate:\n");
  printf("- input_size: %d\n", input_size);
  printf("- allowed_threads_by_elements: %d\n", allowed_threads_by_elements);
  printf("- allowed_threads_by_hardware: %d\n", allowed_threads_by_hardware);
  printf("- num_threads: %d\n", num_threads);
  printf("- block_size: %d\n", block_size);

  // Thread Pool
  std::vector<T> results(num_threads);
  std::vector<std::thread> threads(num_threads - 1);

  // Launch Threads
  InputIt end;
  for (int i = 0; i < num_threads - 1; ++i) {
    // update endpoint
    end = first;
    std::advance(end, block_size);

    // spawn
    threads[i] = std::thread(accumulate<InputIt, T>, first, end, init,
                             std::ref(results[i]));

    // update startpoint
    first = end;
  }

  // last thread is the current one.
  end = first;
  std::advance(end, block_size);
  results[num_threads - 1] = std::accumulate(first, end, init);

  // Join All
  // https://en.cppreference.com/w/cpp/algorithm/for_each
  // https://en.cppreference.com/w/cpp/utility/functional/mem_fn
  // https://stackoverflow.com/questions/37259529/why-use-mem-fn
  std::for_each(threads.begin(), threads.end(),
                std::mem_fn(&std::thread::join));

  // accumulate results
  return std::accumulate(results.begin(), results.end(), init);
}

int main() {
  sequential_accumulate_test();

  const int size = 8000;
  std::vector<int> values(size, 1);

  int sum = parallel_accumulate(values.begin(), values.end(), 0);
  printf("parallel accumulate sum: %d\n", sum);

  return 0;
}
