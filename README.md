# cpp_concurrency
 Code for the Modern C++ Concurrency in Depth course from Udemy. 

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

## Building the code

```bash
sudo apt install gcc-10 g++-10 # C++20

mkdir -p build && cd build
cmake -D CMAKE_C_COMPILER=gcc-10 -D CMAKE_CXX_COMPILER=g++-10 .. && make

-std=c++20 -fcoroutines -pthread
```
