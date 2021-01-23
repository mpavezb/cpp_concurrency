#include <chrono>
#include <cstdio>
#include <thread>

void foo() {
  printf("Hello from function foo.\n");
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void bar() {
  printf("Hello from function bar.\n");
  std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}

int main() {

  std::thread foo_thread(foo);
  std::thread bar_thread(bar);

  bar_thread.detach();
  printf("-> bar detach.\n");

  foo_thread.join();
  printf("-> foo join.\n");

  // program does not wait for bar_thread to finish.
  return 0;
}
