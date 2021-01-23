#include <cstdio>
#include <thread>

void foo() { printf("Hello from foo.\n"); }

void bar() { printf("Hello from bar.\n"); }

int main() {
  std::thread thread_1(foo);
  std::thread thread_2;

  // Copy Disabled.
  // std::thread thread_2 = thread_1;

  // Move Enabled
  std::thread thread_3(std::move(thread_1)); // move ctor with named variable
  std::thread thread_4(std::thread(bar));    // move ctor with tmp variable
  thread_1 = std::move(thread_3);            // move assgn of named variable
  thread_2 = std::thread(bar);               // move assgn of tmp variable

  // Reassign unjoined thread will result in std::terminate!
  // thread_3 = std::move(thread_1);

  thread_1.join();
  thread_2.join();

  return 0;
}
