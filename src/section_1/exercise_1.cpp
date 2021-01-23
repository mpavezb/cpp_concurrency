#include <cstdio>
#include <iostream>
#include <ostream>
#include <thread>

void test() { printf("Hello from function test\n"); }

void functionA() {
  std::thread thread_test(test);
  printf("Hello from functionA\n");
  thread_test.join();
}

void functionB() { printf("Hello from functionB\n"); }

int main() {
  std::thread thread_a(functionA);
  std::thread thread_b(functionB);
  thread_a.join();
  thread_b.join();
  return 0;
}
