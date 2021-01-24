#include <iostream>
#include <list>
#include <mutex>
#include <thread>

std::list<int> my_list;
std::mutex m1, m2;

void get_size() {
  m1.lock();
  int size = my_list.size();
  m1.unlock();
  std::cout << "size of the list is : " << size << std::endl;
}

void add_to_list__mutex(int const &x) {
  m1.lock();
  my_list.push_front(x);
  m1.unlock();
}

void add_to_list__lock_guard(int const &x) {
  std::lock_guard<std::mutex> lock(m1);
  my_list.push_front(x);
}

void add_to_list__scoped_lock(int const &x) {
  std::scoped_lock<std::mutex, std::mutex> lock(m1, m2);
  my_list.push_front(x);
}

int main() {
  std::thread thread_1;
  std::thread thread_2;

  // access is guaranteed to be mutually exclusive
  thread_1 = std::thread(add_to_list__mutex, 4);
  thread_2 = std::thread(get_size);
  thread_1.join();
  thread_2.join();

  // RAII style - lock guard
  thread_1 = std::thread(add_to_list__lock_guard, 4);
  thread_2 = std::thread(get_size);
  thread_1.join();
  thread_2.join();

  // RAII style - lock guard
  thread_1 = std::thread(add_to_list__scoped_lock, 4);
  thread_2 = std::thread(get_size);
  thread_1.join();
  thread_2.join();
}
