#include <atomic>
#include <cassert>
#include <cstdio>
#include <iostream>
#include <string>
#include <thread>

// ============================================================================
// EXAMPLE: Release-Acquire Protocol
// =============================================================================
std::atomic<std::string *> ptr;
int data;

void rel_acq_producer() {
  std::string *p = new std::string("Hello");
  data = 42;
  ptr.store(p, std::memory_order_release);
}

void rel_acq_consumer() {
  std::string *p2;
  while (!(p2 = ptr.load(std::memory_order_acquire)))
    ;
  std::cout << "Acquired string '" << *p2 << "'" << std::endl;
  std::cout << "Acquired data   '" << data << "'" << std::endl;

  assert(*p2 == "Hello"); // never fires
  assert(data == 42);     // never fires
}

void release_acquire_example() {
  std::thread t1(rel_acq_producer);
  std::thread t2(rel_acq_consumer);
  t1.join();
  t2.join();
}

// =============================================================================
// EXAMPLE: Release-Consume Protocol
// =============================================================================
std::atomic<std::string *> ptr2;
int data2;

void rel_con_producer() {
  std::string *p = new std::string("Hello");
  data2 = 42;
  ptr2.store(p, std::memory_order_release);
}

void rel_con_consumer() {
  std::string *p2;
  while (!(p2 = ptr2.load(std::memory_order_consume)))
    ;

  std::cout << "Consumed string '" << *p2 << "'" << std::endl;
  std::cout << "Consumed data   '" << data2 << "'" << std::endl;

  assert(*p2 == "Hello"); // never fires: *p2 carries dependency from ptr2
  assert(data2 == 42);    // may or may not fire: data2 does not carry a
                          // dependency from ptr2
}

void release_consume_example() {
  std::thread t1(rel_con_producer);
  std::thread t2(rel_con_consumer);
  t1.join();
  t2.join();
}

// =============================================================================

int main() {
  release_acquire_example();
  release_consume_example();
}
