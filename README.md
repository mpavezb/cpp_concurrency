# cpp_concurrency

Code for the Modern C++ Concurrency in Depth course from Udemy. 

## Resources

- https://en.cppreference.com/w/cpp/thread/thread
- https://en.cppreference.com/w/cpp/utility/functional/ref
- Course source code: https://github.com/kasunindikaliyanage/cpp_concurrency_masterclass

## Concepts

Process: Is defined by its instructions and current state.

Context Switching:
- The OS can store the state of a process or thread, so that it can be restored and resume execution at a later point. 
- This allows multiple processes to share a single central processing unit (CPU), and is an essential feature of a multitasking operating system.

Round-Robin Scheduling:
- Assigns time-slots to each process in equal portions and circular order, having no priorities.
- Context switching is performed when a time slot is completed.
- Simple and starvation-free.
- See: https://en.wikipedia.org/wiki/Round-robin_scheduling

Task- and Data- Level Parallelism:
- Task-Level: Threads perform different tasks (using the same or different data).
- Data-Level: Threads perform the same task on different data.

Parallelism vs Concurrency:
- Parallelism: When each process/thread runs on a dedicated processor at the same time.
- Concurrency: Emulated parallelism based on context-switching (sequencial execution!).
- True parallelism is difficult to achieve, due to the low number of cores usually available.

Heterogeneous Computing:
- Refers to systems that use more than one kind of processor or cores. 
- These systems gain performance or energy efficiency by using specialized hardware.
- https://en.wikipedia.org/wiki/Heterogeneous_computing

GPGPU (General Purpose GPU):
- Is the use of a GPU to handle tasks a CPU would do.
- The idea is to handle matrix-like data using the GPU.
- See: https://en.wikipedia.org/wiki/General-purpose_computing_on_graphics_processing_units

## Threads

Thread Object: A properly constructed thread object represents an active thread of execution in hardware level.

### Thread Management

Joinability: 
- *Properly constructed* thread objects are *joinable*. Default constructed are *non-joinable*!.
- Calling the `join` or `detach` makes the object *non-joinable*.
- If `join` or `detach` are not called, then `std::terminate` will be called by the destructor.
- Programs having calls to `std::terminate` (aborts program) are referred as *unsafe*.
- See: [example](src/section_1/01_joinability.cpp).

`join()`:
- Introduces a *synchronization point* between the thread and the caller.
- It blocks the execution of the caller, until the thread execution finishes.

`detach()`:
- Separates the thread from the thread object, allowing the thread to continue independenly.
- Detached threads may outlive parents.
- Detached threads are *non-joinable*, thus they can be safely destroyed.
- Detached threads usually cannot outlive the `main()` function, as the OS terminates all threads on exit [stackoverflow.com](https://stackoverflow.com/questions/19744250/what-happens-to-a-detached-thread-when-main-exits).
- Allocated resources will be freed once the thread finishes.
- Be wary of passing by reference to detached threads!. When owner finishes, reference will dangle.
- See: [example](src/section_1/02_detach.cpp).

Delayed Joins and Exceptions:
- We might need to delay the call to `join` to not block the thread so early. Notice that we can call `detach` as soon as we want!.
- If an exception is thrown before `join`, `std::terminate` might be called!.
- RAII can be used to handle the thread resource and join when necessary.
- See: [example](src/section_1/03_exceptions.cpp).

[Thread Constructor](https://en.cppreference.com/w/cpp/thread/thread/thread):
- Default creates non-joinable thread.
- Move enabled and Copy Disabled (see: [example](src/section_1/06_ownership.cpp)).
- Class/Fn arguments are forwarded. Use `std::ref` to pass by reference. (see: [example](src/section_1/04_thread_parameters.cpp), [example](src/section_1/05_pass_by_ref_and_detach.cpp)).

Useful API ([example](src/section_1/07_useful_api.cpp)):
- [get_id()](https://en.cppreference.com/w/cpp/thread/thread/get_id): Unique id for *active* threads, contant otherwise. The type `std::thread::id` is designed to be used as a key in associative containers.
- [sleep_for()](https://en.cppreference.com/w/cpp/thread/sleep_for): Blocks execution for *at least* the specified duration. It may block longer due to scheduling or resource contention delays.
- [yield()](https://en.cppreference.com/w/cpp/thread/yield): Hints the scheduler to allow other threads to run, and re-inserts the thread into the scheduling queue.
- [hardware_concurrency()](https://en.cppreference.com/w/cpp/thread/thread/hardware_concurrency): Returns the number of concurrent threads supported by the implementation (logical cores). The value should be considered only a hint.


## TODO

- jthread
- futures
- latches and barriers
- mutex
- C++20 addons

## Building the code

```bash
sudo apt install gcc-10 g++-10 # C++20

mkdir -p build && cd build
cmake .. && make && src/section_1/01_joinability

#cmake -D CMAKE_C_COMPILER=gcc-10 -D CMAKE_CXX_COMPILER=g++-10 .. && make
#-std=c++20 -fcoroutines -pthread
```
