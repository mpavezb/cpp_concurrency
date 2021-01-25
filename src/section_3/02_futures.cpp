#include <chrono>
#include <future>
#include <iostream>
#include <thread>

int find_answer_how_old_universe_is() {
  std::this_thread::sleep_for(std::chrono::seconds(5));
  return 5;
}

void do_other_calculations() { std::cout << "Doing other stuff " << std::endl; }

int main() {
  std::future<int> the_answer_future =
      std::async(find_answer_how_old_universe_is);

  // ....
  do_other_calculations();
  // ...

  std::cout << "The universe age is " << the_answer_future.get() << "s."
            << std::endl;
}
