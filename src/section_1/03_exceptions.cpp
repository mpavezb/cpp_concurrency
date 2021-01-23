#include <chrono>
#include <cstdio>
#include <stdexcept>
#include <thread>

void foo() { printf("Hello from function foo.\n"); }

void hazard() {
  printf("Throwing operation\n");
  throw std::runtime_error("Runtime Error in hazard() operation");
}

// thread_guard implements RAII for std::thread.
class thread_guard {
  std::thread &t;

public:
  // no implicit conversions
  explicit thread_guard(std::thread &_t) : t(_t) {}

  // calls join
  ~thread_guard() {
    if (t.joinable()) {
      t.join();
    }
  }

  // non-copiable.
  thread_guard(thread_guard const &) = delete;
  thread_guard &operator=(thread_guard const &) = delete;
};

void example_try_catch() {
  std::thread foo_thread(foo);
  // Using try catch solves the issue, but exceptions could still be thrown on
  // more complex scenarios where try/catch is forgotten.
  try {
    hazard();
    foo_thread.join();
  } catch (...) {
    foo_thread.join();
  }
}

void example_raii() {
  std::thread foo_thread(foo);
  thread_guard tg(foo_thread);

  try {
    hazard();
  } catch (...) {
  }
}

int main() {
  example_try_catch();
  example_raii();
  return 0;
}
