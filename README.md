# cpp_concurrency

Code for the Modern C++ Concurrency in Depth course from Udemy. 

## Resources

- https://en.cppreference.com/w/cpp/thread/thread/thread
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

`join()`:
- Introduces a *synchronization point* between the thread and the caller.
- It blocks the execution of the caller, until the thread execution finishes.

`detach()`:
- Separates the thread from the thread object, allowing the thread to continue independenly.
- Detached threads can operate even after the launcher has finished. Notice that detached threads are *non-joinable*, thus they can be safely destroyed.
- Allocated resources will be freed once the thread finishes.

Delayed Joins and Exceptions:
- We might need to delay the call to `join` to not block the thread so early. Notice that we can call `detach` as soon as we want!.
- If an exception is thrown before `join`, `std::terminate` might be called!.
- RAII can be used to handle the thread resource and join when necessary.


Thread Constructor:
1. `thread() noexcept;`: Default creates non-joinable thread.
2. `thread( thread&& other ) noexcept;` Moves thread.
3. `template< class Function, class... Args > explicit thread( Function&& f, Args&&... args );`: Arguments are forwarded to the constructor. Use `std::ref` to pass by reference.
4. `thread( const thread& ) = delete;` Non-Copiable.

## Building the code

```bash
sudo apt install gcc-10 g++-10 # C++20

mkdir -p build && cd build
cmake -D CMAKE_C_COMPILER=gcc-10 -D CMAKE_CXX_COMPILER=g++-10 .. && make

-std=c++20 -fcoroutines -pthread
```
