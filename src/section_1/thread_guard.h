#ifndef THREAD_GUARD_H_
#define THREAD_GUARD_H_

#include <thread>

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

#endif /* THREAD_GUARD_H_ */
