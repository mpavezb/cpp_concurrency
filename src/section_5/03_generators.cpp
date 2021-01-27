#include <cassert>
#include <coroutine>
#include <cstdio>
#include <exception>

// ===================================================================
// User defined Generator type and promise_types.
// ===================================================================

template <typename T> class Generator {
public:
  struct promise_type;

  using co_handle = std::coroutine_handle<promise_type>;

  Generator(co_handle handle) : handle_(handle) {}
  ~Generator() { handle_.destroy(); }
  Generator(Generator &) = delete;
  Generator(Generator &&) = delete;

  T getValue() { return handle_.promise().current_value; }

  bool next() {
    handle_.resume();
    return not handle_.done();
  }

private:
  co_handle handle_;
};

template <typename T> struct Generator<T>::promise_type {
  using co_handle = std::coroutine_handle<promise_type>;

  auto get_return_object() { return co_handle::from_promise(*this); }

  auto initial_suspend() { return std::suspend_always(); }
  auto final_suspend() { return std::suspend_always(); }
  void return_void() {}
  void unhandled_exception() { std::terminate(); }

  // enable co_yield operator
  auto yield_value(const T value) {
    current_value = value;
    return std::suspend_always();
  }

  T current_value;
};
// ===================================================================

Generator<int> genInts(int start = 0, int step = 1) noexcept {
  auto value = start;
  for (int i = 0;; ++i) {
    co_yield value;
    value += step;
  }
}

int main() {
  auto gen = genInts();
  for (int i = 0; i <= 10; ++i) {
    gen.next();
    printf("Generating integer: %d\n", gen.getValue());
  }

  return 0;
}
