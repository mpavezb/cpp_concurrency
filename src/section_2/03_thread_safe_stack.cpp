#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <mutex>
#include <stack>
#include <thread>

/**
 * If we take std::stack and wrap it adding mutexes to each method, we can
 * achieve basic thread safety. However, it limits the true parallel access to
 * the container, as only one thread could do any operation at once.
 *
 */
template <typename T> class thread_safe_stack {
  std::stack<T> stack;
  std::mutex mutex;

public:
  void push(T element) {
    std::lock_guard<std::mutex> lock(mutex);
    stack.push(element);
  }

  void pop() {
    std::lock_guard<std::mutex> lock(mutex);
    stack.pop();
  }

  T &top() {
    std::lock_guard<std::mutex> lock(mutex);
    return stack.top();
  }

  bool empty() {
    std::lock_guard<std::mutex> lock(mutex);
    return stack.empty();
  }

  size_t size() {
    std::lock_guard<std::mutex> lock(mutex);
    return stack.size();
  }
};

int getRandZeroOrOne() { return rand() > (RAND_MAX / 2); }

/**
 * Race condition inherited from the stack interface:
 *
 * Setup: Stack with 1 element.
 * Thread 1: Accesses the if statement.
 * Thread 2: Evaluates whole function.
 * Thread 1: Race condition when doing top().
 */
void race_condition_example() {
  thread_safe_stack<int> stack;
  stack.push(0);

  auto check_and_pop = [&stack]() {
    if (not stack.empty()) {

      // simulate race condition
      std::this_thread::sleep_for(std::chrono::seconds(getRandZeroOrOne()));

      int value = stack.top();
      printf("The top value is: %d\n", value);
      stack.pop();
    }
  };

  std::thread t1(check_and_pop);
  std::thread t2(check_and_pop);
  t1.join();
  t2.join();
}

int main() {
  printf("Rand 0 or 1 test: %d\n", getRandZeroOrOne());
  printf("Rand 0 or 1 test: %d\n", getRandZeroOrOne());
  printf("Rand 0 or 1 test: %d\n", getRandZeroOrOne());
  printf("Rand 0 or 1 test: %d\n", getRandZeroOrOne());

  race_condition_example();
  return 0;
}
