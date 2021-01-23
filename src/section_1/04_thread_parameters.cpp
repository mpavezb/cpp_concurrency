#include <chrono>
#include <cstdio>
#include <thread>

void foo_by_value(int x, int y) { printf("%d + %d = %d\n", x, y, x + y); }

void foo_by_ref(int const &n) {
  while (true) {
    printf("N = %d\n", n);
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}

int main() {

  // ============================================
  // Pass by Value
  // ============================================
  int x = 10;
  int y = 5;
  std::thread thread_by_value(foo_by_value, x, y);
  thread_by_value.join();

  // ============================================
  // Pass by Reference
  // ============================================
  int N = 10;
  printf("N = %d\n", N);

  // Explicitly wrap as reference, otherwise the variable is passed by value.
  std::thread thread_by_ref(foo_by_ref, std::ref(N));

  std::this_thread::sleep_for(std::chrono::seconds(5));
  N = 15;
  printf("N = %d\n", N);
  thread_by_ref.join();

  return 0;
}
