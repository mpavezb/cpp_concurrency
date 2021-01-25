#include <chrono>
#include <future>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <thread>

// ===================================================================
// EXAMPLE 1: Shared Future.
// - Calling multiple get() results in exception.
// ===================================================================
void print_result1(std::future<int> &fut) {
  // Exception when called multiple times!
  // std::cout << fut.get() << "\n";

  // The problem persists even if checking beforehand, because of race
  // condition between valid() and get().
  if (fut.valid()) {
    std::cout << "this is valid future\n";
    std::cout << fut.get() << "\n";
  } else {
    std::cout << "this is invalid future\n";
  }
}

void run_code1() {
  std::promise<int> prom;
  std::future<int> fut(prom.get_future());

  std::thread th1(print_result1, std::ref(fut));
  std::thread th2(print_result1, std::ref(fut));

  prom.set_value(5);

  th1.join();
  th2.join();
}

// ===================================================================
// EXAMPLE 2: std::shared_future
// ===================================================================

void print_result2(std::shared_future<int> &fut) {
  std::cout << fut.get() << "  - valid future \n";
}

void run_code2() {
  std::promise<int> prom;
  std::shared_future<int> fut(prom.get_future());

  std::thread th1(print_result2, std::ref(fut));
  std::thread th2(print_result2, std::ref(fut));

  prom.set_value(5);

  th1.join();
  th2.join();
}

int main() {
  // run_code1(); // <-- Exception
  run_code2();

  return 0;
}
