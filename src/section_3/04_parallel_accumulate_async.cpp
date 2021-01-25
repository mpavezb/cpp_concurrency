#include <future>
#include <iostream>
#include <numeric>
#include <vector>

int MIN_ELEMENT_COUNT = 1000;

// Async version using Divide-and-Conquer
template <typename iterator>
int parallel_accumulate_async(iterator begin, iterator end) {
  // Base Case
  long length = std::distance(begin, end);
  if (length <= MIN_ELEMENT_COUNT) {
    std::cout << " - base case on thread: " << std::this_thread::get_id()
              << std::endl;
    return std::accumulate(begin, end, 0);
  }

  // Recurse
  iterator mid = begin;
  std::advance(mid, (length + 1) / 2);
  std::future<int> f1 =
      std::async(std::launch::deferred | std::launch::async,
                 parallel_accumulate_async<iterator>, mid, end);
  auto sum = parallel_accumulate_async(begin, mid);
  return sum + f1.get();
}

int main() {
  std::vector<int> v(10000, 1);
  std::cout << "The sum is " << parallel_accumulate_async(v.begin(), v.end())
            << '\n';
}
