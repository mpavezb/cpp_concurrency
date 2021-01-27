#include <chrono>
#include <cstdio>
#include <thread>

using namespace std::literals;

void non_interruptible_counter() {
  int counter{0};
  while (counter < 10) {
    std::this_thread::sleep_for(0.2s);
    printf("This is an non-interruptible thread. Counter: %d\n", counter);
    ++counter;
  }
}

void interruptible_counter(std::stop_token token) {
  int counter{0};
  while (counter < 10) {

    // Interupt Point!
    if (token.stop_requested()) {
      return;
    }

    std::this_thread::sleep_for(0.2s);
    printf("This is an interruptible thread. Counter: %d\n", counter);
    ++counter;
  }
}

int main() {
  // No join() is needed!.
  std::jthread thread1([]() { printf("Hello from jthread\n"); });

  // Interruption Example
  std::jthread non_interruptible_thread(non_interruptible_counter);
  std::jthread interruptible_thread(
      interruptible_counter); // Token is automatic
  std::this_thread::sleep_for(1s);
  non_interruptible_thread.request_stop();
  interruptible_thread.request_stop();

  return 0;
}
