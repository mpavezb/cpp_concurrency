#include <cstdio>
#include <iostream>
#include <thread>

void clean() {
  printf("Cleaning ...\n");
  std::this_thread::sleep_for(std::chrono::milliseconds(5000));
  printf("Cleaning completed!\n");
}

void engineFullSpeed() {
  printf("Setting engine at full speed ...\n");
  std::this_thread::sleep_for(std::chrono::milliseconds(5000));
  printf("Engine is at full speed!\n");
}

void engineStop() {
  printf("Stopping engine ...\n");
  std::this_thread::sleep_for(std::chrono::milliseconds(5000));
  printf("Engine is stopped!\n");
}

int main() {

  int command_no;
  while (true) {
    std::cout << "\nEnter a command {1=clean,2=full_speed,3=stop,100=exit} : ";
    std::cin >> command_no;
    if (command_no == 1) {
      printf("<clean>.\n");
      std::thread cleaner_thread(clean);
      cleaner_thread.detach();

    } else if (command_no == 2) {
      printf("<full_speed>.\n");
      std::thread engine_thread(engineFullSpeed);
      engine_thread.join();

    } else if (command_no == 3) {
      printf("<stop>.\n");
      std::thread engine_thread(engineStop);
      engine_thread.join();

    } else if (command_no == 100) {
      printf("<exit>.\n");
      break;

    } else {
      printf("<unknown command>\n");
    }
  }
  return 0;
}
