#include <chrono>
#include <cstdio>
#include <iostream>
#include <thread>

void foo() {
  std::cout << "This thread id: " << std::this_thread::get_id() << std::endl;
}

void get_id_test() {
  std::thread thread_1(foo);
  std::thread thread_2(foo);
  std::thread thread_3(foo);
  std::cout << "thread_1 id: " << thread_1.get_id() << std::endl;
  std::cout << "thread_2 id: " << thread_2.get_id() << std::endl;
  std::cout << "thread_3 id: " << thread_3.get_id() << std::endl;
  thread_1.join();
  thread_2.join();
  thread_3.join();

  // ID of non-executing thread.
  std::thread thread_4;
  std::cout << "thread_4 id: " << thread_4.get_id() << std::endl;
}

void display_hardware_concurrency() {
  std::cout << "Allowed max number of parallel threads : "
            << std::thread::hardware_concurrency() << std::endl;
}

thread_local int sample_local_integer = 0;
int sample_normal_integer = 0;

void thread_local_sample() {
  auto fn = []() {
    ++sample_local_integer;
    ++sample_normal_integer;
    printf("- local integer: %d, normal integer: %d\n", sample_local_integer,
           sample_normal_integer);
  };
  std::thread t1(fn);
  std::thread t2(fn);
  std::thread t3(fn);

  t1.join();
  t2.join();
  t3.join();
}

int main() {
  get_id_test();
  display_hardware_concurrency();
  thread_local_sample();
}
