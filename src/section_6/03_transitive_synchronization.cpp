#include <assert.h>
#include <atomic>
#include <thread>

std::atomic<int> data[5];
std::atomic<bool> sync1{false}, sync2{false};

// Writer Thread:
// - Stores data using order relaxed.
// - Sends completion signal "sync1" using order release.
void thread_f1() {
  data[0].store(42, std::memory_order_relaxed);
  data[1].store(45, std::memory_order_relaxed);
  data[2].store(47, std::memory_order_relaxed);
  data[3].store(49, std::memory_order_relaxed);
  data[4].store(56, std::memory_order_relaxed);
  sync1.store(true, std::memory_order_release);
}

// Synchronization Thread:
// - Syncs completion flag "sync1" with readiness flag "sync2".
void thread_f2() {
  while (!sync1.load(std::memory_order_acquire))
    ;
  sync2.store(true, std::memory_order_release);
}

// Consumer Thread:
// - Waits ready signal "sync2" using order acquire.
// - Reads data using order relaxed.
void thread_f3() {
  while (!sync2.load(std::memory_order_acquire))
    ;
  assert(data[0].load(std::memory_order_relaxed) == 42);
  assert(data[1].load(std::memory_order_relaxed) == 45);
  assert(data[2].load(std::memory_order_relaxed) == 47);
  assert(data[3].load(std::memory_order_relaxed) == 49);
  assert(data[4].load(std::memory_order_relaxed) == 56);
}

int main() {
  std::thread thread_1(thread_f1);
  std::thread thread_2(thread_f2);
  std::thread thread_3(thread_f3);

  thread_1.join();
  thread_2.join();
  thread_3.join();
  return 0;
}
