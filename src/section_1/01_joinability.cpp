#include <cstdio>
#include <thread>

void test() { printf("Hello from function test\n"); }

void check_joinable(std::thread &a_thread) {
  if (a_thread.joinable()) {
    printf("Thread Object is joinable.\n");
  } else {
    printf("Thread Object is not joinable.\n");
  }
}

int main() {

  std::thread a_thread(test);
  check_joinable(a_thread); // before join.
  a_thread.join();
  check_joinable(a_thread); // after join.

  // not properly constructed => not joinable.
  std::thread default_thread;
  check_joinable(default_thread);

  return 0;
}
