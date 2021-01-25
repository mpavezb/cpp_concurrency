#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

int total_distance = 5;
int distance_covered = 0;

std::condition_variable cv;
std::mutex mutex;

void driver() {
  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    distance_covered++;
    printf("Driver: Moved 1m forward. distance_covered=%d\n", distance_covered);

    if (distance_covered == total_distance) {
      printf("Driver: Arrived!, Will notify him.\n");
      cv.notify_one();
      break;
    }
  }
}

void passenger() {
  std::unique_lock<std::mutex> lock(mutex);

  // This predicated overload takes care of everything.
  cv.wait(lock, [] { return distance_covered == total_distance; });

  printf("Passenger: I am there!, distance_covered=%d\n", distance_covered);
}

int main() {
  std::thread driver_thread(driver);
  std::thread passener_thread(passenger);
  passener_thread.join();
  driver_thread.join();
}
