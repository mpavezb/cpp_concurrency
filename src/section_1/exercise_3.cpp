#include <chrono>
#include <cstdio>
#include <iostream>
#include <queue>
#include <thread>

void cleaners(std::queue<bool> &queue) {
  while (true) {
    if (!queue.empty()) {
      queue.pop();
      printf("Cleaning ...\n");
      std::this_thread::sleep_for(std::chrono::seconds(1));
    } else {
      printf("No cleaning orders ...\n");
      std::this_thread::sleep_for(std::chrono::seconds(2));
    }
  }
}

void workers(std::queue<bool> &queue) {
  while (true) {
    if (!queue.empty()) {
      queue.pop();
      printf("Working ...\n");
      std::this_thread::sleep_for(std::chrono::seconds(1));
    } else {
      printf("No work orders ...\n");
      std::this_thread::sleep_for(std::chrono::seconds(2));
    }
  }
}

int main() {

  std::queue<bool> clean_queue;
  std::queue<bool> work_queue;

  std::thread cleaners_thread(cleaners, std::ref(clean_queue));
  std::thread workers_thread(workers, std::ref(work_queue));

  cleaners_thread.detach();
  workers_thread.detach();

  printf("Starting ... \n");
  std::this_thread::sleep_for(std::chrono::seconds(1));

  int command_no;
  while (true) {
    std::cout << "\nEnter a command {1=clean,2=work,100=exit} : ";
    std::cin >> command_no;
    if (command_no == 1) {
      printf("<clean>\n");
      clean_queue.push(true);

    } else if (command_no == 2) {
      printf("<work>\n");
      work_queue.push(true);

    } else if (command_no == 100) {
      printf("<exit>.\n");
      break;

    } else {
      printf("<unknown command>\n");
    }
  }
  return 0;
}
