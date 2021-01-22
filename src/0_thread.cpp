#include <cstdio>
#include <iostream>
#include <ostream>
#include <thread>

void foo() {
  auto id = std::this_thread::get_id();
  std::cout << "Hello from funciton foo. Thread ID:" << id << std::endl;
}

class Bar {
public:
  void operator()() {
    auto id = std::this_thread::get_id();
    std::cout << "Hello from functor Bar.Thread ID:" << id << std::endl;
  }
};

int main() {
  std::thread thread1(foo);

  Bar bar;
  std::thread thread2(bar);

  std::thread thread3([] {
    auto id = std::this_thread::get_id();
    std::cout << "Hello from lambda.Thread ID:" << id << std::endl;
  });

  thread1.join();
  thread2.join();
  thread3.join();
  std::cout << "Hello from main." << std::endl;
  return 0;
}
