#include <cassert>
#include <coroutine>
#include <cstdio>
#include <exception>

// ===================================================================
// User Defined resumable and promise_types. They are generally the
// same for every case.
// ===================================================================
class resumable {
public:
  struct promise_type;

  using co_handle = std::coroutine_handle<promise_type>;

  resumable(co_handle handle) : handle_(handle) { assert(handle); }
  ~resumable() { handle_.destroy(); }
  resumable(resumable &) = delete;
  resumable(resumable &&) = delete;

  bool resume() {
    if (not handle_.done()) {
      handle_.resume();
    }
    return not handle_.done();
  }

private:
  co_handle handle_;
};

struct resumable::promise_type {
  using co_handle = std::coroutine_handle<promise_type>;

  auto get_return_object() { return co_handle::from_promise(*this); }

  auto initial_suspend() { return std::suspend_always(); }
  auto final_suspend() { return std::suspend_always(); }
  void return_void() {}
  void unhandled_exception() { std::terminate(); }
};
// ===================================================================

resumable foo() {
  printf("<foo> a\n");
  co_await std::suspend_always();
  printf("<foo> b\n");
  co_await std::suspend_always();
  printf("<foo> c\n");
}

int main() {
  // Coroutine is created in initial suspend state.
  resumable r1 = foo();
  r1.resume();
  r1.resume();
  r1.resume();
  r1.resume();
  return 0;
}
