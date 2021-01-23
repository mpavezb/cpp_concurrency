#include <chrono>
#include <cstdio>
#include <functional>
#include <thread>

void foo_by_value(int x, int y) { printf("%d + %d = %d\n", x, y, x + y); }

void foo_by_ref(int const &n) {
  while (true) {
    printf("N = %d\n", n);
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}

int main() {

  int x = 10;
  int y = 5;
  std::thread thread_1(foo_by_value, x, y);
  thread_1.join();

  int N = 10;
  printf("N = %d\n", N);

  // Explicitly wrap as reference, otherwise the variable is passed by value.
  std::thread thread_2(foo_by_ref, std::ref(N));

  std::this_thread::sleep_for(std::chrono::seconds(5));

  N = 15;
  printf("N = %d\n", N);

  thread_2.join();

  return 0;
}
