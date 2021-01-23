#include <chrono>
#include <cstdio>
#include <exception>
#include <stdexcept>
#include <thread>

void zork(int const &N) {
  while (true) {
    try {
      // Detached thread will throw when trying to access freed memory.
      printf("N = %d\n", N);
      std::this_thread::sleep_for(std::chrono::seconds(1));
    } catch (...) {
      throw std::runtime_error("Runtime Error on zork");
    }
  }
}

void bar() {
  int N = 5;
  std::thread zork_thread(zork, std::ref(N));
  zork_thread.detach();

  std::this_thread::sleep_for(std::chrono::seconds(5));
  printf("bar() has finished.\n");
}

int main() {
  std::thread bar_thread(bar);
  bar_thread.join();

  return 0;
}
