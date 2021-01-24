#include <cstdio>
#include <iostream>
#include <list>
#include <mutex>
#include <thread>

// =========================================================
class list_wrapper {
  std::list<int> my_list;
  std::mutex m;

public:
  void add_to_list(int const &x) {
    std::lock_guard<std::mutex> lg(m);
    my_list.push_front(x);
  }

  void size() {
    std::lock_guard<std::mutex> lg(m);
    int size = my_list.size();
    std::cout << "size of the list is : " << size << std::endl;
  }

  // PROBLEM: Function breaks mutex locking of shared resource.
  std::list<int> *get_data() { return &my_list; }
};

// =========================================================

struct Data {
  void foo() { printf("Hello from foo()\n"); }
};

class DataWrapper {
  Data data;
  std::mutex m;

public:
  // Allows running any function on the data that should be protected.
  // In particular, the caller can retain a reference to the data.
  template <typename Fn> void bar(Fn f) {
    // Guard cannot guarantee safe usage of data.
    std::lock_guard<std::mutex> lg(m);
    f(data);
  }
};

Data *unprotected_data;
void malicious_function(Data &data) { unprotected_data = &data; }

int main() {
  DataWrapper wrapper;
  wrapper.bar(malicious_function);
  unprotected_data->foo();
}
