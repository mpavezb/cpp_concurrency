# cpp_concurrency

Code for the Modern C++ Concurrency in Depth course from Udemy. 

## Resources

- https://en.cppreference.com/w/cpp/thread/thread
- https://en.cppreference.com/w/cpp/utility/functional/ref
- Course source code: https://github.com/kasunindikaliyanage/cpp_concurrency_masterclass
- C++ Compiler Support: https://en.cppreference.com/w/cpp/compiler_support

Relevant Talks:
- [CppCon 2017: Fedor Pikus, C++ atomics, from basic to advanced. What do they really do?](https://www.youtube.com/watch?v=ZQFzMfHIxng): How atomics work, atomic (lock free) vs mutex-based, dealing with issues.

## Building the code

```bash
sudo apt install gcc-10 g++-10 # C++20

mkdir -p build && cd build
cmake -D CMAKE_C_COMPILER=gcc-10 -D CMAKE_CXX_COMPILER=g++-10 .. && make && src/section_1/01_joinability
```

## C++ Thread Support Library

- [Basic Concepts](#basic-concepts).
- [Thread Management](#thread-management).
- [Locking Mechanisms](#locking-mechanisms).
- [Condition Variables and Futures](#condition-variables-and-futures).
- [STL Containers and Algorithms](#stl-containers-and-algorithms).
- [C++20 Addons](#c-20-addons).
- [Memory Model and Atomic Operations](#memory-model-and-atomic-operations).
- [Lock Free Data Structures And Algorithms](#lock-free-data-structures-and-algorithms).

### Basic Concepts

**Process:** Is defined by its instructions and current state.

**Context Switching:**
- The OS can store the state of a process or thread, so that it can be restored and resume execution at a later point. 
- This allows multiple processes to share a single central processing unit (CPU), and is an essential feature of a multitasking operating system.

**Round-Robin Scheduling:**
- Assigns time-slots to each process in equal portions and circular order, having no priorities.
- Context switching is performed when a time slot is completed.
- Simple and starvation-free.
- See: https://en.wikipedia.org/wiki/Round-robin_scheduling

**Task- and Data- Level Parallelism:**
- Task-Level: Threads perform different tasks (using the same or different data).
- Data-Level: Threads perform the same task on different data.

**Parallelism vs Concurrency:**
- Parallelism: When each process/thread runs on a dedicated processor at the same time.
- Concurrency: Emulated parallelism based on context-switching (sequencial execution!).
- True parallelism is difficult to achieve, due to the low number of cores usually available.

**Heterogeneous Computing:**
- Refers to systems that use more than one kind of processor or cores. 
- These systems gain performance or energy efficiency by using specialized hardware.
- https://en.wikipedia.org/wiki/Heterogeneous_computing

**GPGPU (General Purpose GPU):**
- Is the use of a GPU to handle tasks a CPU would do.
- The idea is to handle matrix-like data using the GPU.
- See: https://en.wikipedia.org/wiki/General-purpose_computing_on_graphics_processing_units

In performance of parallel implementations is better if there is enough hardware to overcome the thread management and context switching costs.

### Thread Management

**Thread Object**: A properly constructed thread object represents an active thread of execution in hardware level.

**Joinability**:
- *Properly constructed* thread objects are *joinable*. Default constructed are *non-joinable*!.
- Calling the `join` or `detach` makes the object *non-joinable*.
- If `join` or `detach` are not called, then `std::terminate` will be called by the destructor.
- Programs having calls to `std::terminate` (aborts program) are referred as *unsafe*.
- See: [example](src/section_1/01_joinability.cpp).

**Join Function** [join()](https://en.cppreference.com/w/cpp/thread/thread/join):
- Introduces a *synchronization point* between the thread and the caller.
- It blocks the execution of the caller, until the thread execution finishes.

**Detach function** [detach()](https://en.cppreference.com/w/cpp/thread/thread/detach):
- Separates the thread from the thread object, allowing the thread to continue independenly.
- Detached threads may outlive parents.
- Detached threads are *non-joinable*, thus they can be safely destroyed.
- Detached threads usually cannot outlive the `main()` function, as the OS terminates all threads on exit [stackoverflow.com](https://stackoverflow.com/questions/19744250/what-happens-to-a-detached-thread-when-main-exits).
- Allocated resources will be freed once the thread finishes.
- Be wary of passing by reference to detached threads!. When owner finishes, reference will dangle.
- See: [example](src/section_1/02_detach.cpp).

**Delayed Joins and Exceptions**:
- We might need to delay the call to `join` to not block the thread so early. Notice that we can call `detach` as soon as we want!.
- If an exception is thrown before `join`, `std::terminate` might be called!.
- RAII can be used to handle the thread resource and join when necessary.
- See: [example](src/section_1/03_exceptions.cpp).

**Thread Constructor** [thread()](https://en.cppreference.com/w/cpp/thread/thread/thread):
- Default creates non-joinable thread.
- Move enabled and Copy Disabled (see: [example](src/section_1/06_ownership.cpp)).
- Class/Fn arguments are forwarded. Use `std::ref` to pass by reference. (see: [example](src/section_1/04_thread_parameters.cpp), [example](src/section_1/05_pass_by_ref_and_detach.cpp)).

**Useful API** ([example](src/section_1/07_useful_api.cpp)):
- [get_id()](https://en.cppreference.com/w/cpp/thread/thread/get_id): Unique id for *active* threads, contant otherwise. The type `std::thread::id` is designed to be used as a key in associative containers.
- [std::this_thread::sleep_for()](https://en.cppreference.com/w/cpp/thread/sleep_for): Blocks execution for *at least* the specified duration. It may block longer due to scheduling or resource contention delays.
- [std::this_thread::yield()](https://en.cppreference.com/w/cpp/thread/yield): Hints the scheduler to allow other threads to run, and re-inserts the thread into the scheduling queue.
- [hardware_concurrency()](https://en.cppreference.com/w/cpp/thread/thread/hardware_concurrency): Returns the number of concurrent threads supported by the implementation (logical cores). The value should be considered only a hint.
- [thread_local](https://en.cppreference.com/w/c/thread/thread_local): Macro specifying that a variable has thread-local storage duration; Each thread has its own, distinct, object. Initialization and destruction are bound to the thread.

**More Examples**:
- Exercise 1: [code](src/section_1/exercise_1.cpp)
- Exercise 2: [code](src/section_1/exercise_2.cpp)
- Exercise 3: [code](src/section_1/exercise_3.cpp)
- Parallel Accumulate: [code](src/section_1/08_parallel_accumulate.cpp).

### Locking Mechanisms

The most common problem in multithreading implementations are broken invariants while updating data.

**Race Condition**: When the outcome depends on the sequence or timing of operations on the threads. They are hard to reproduce and debug, as the operation is non-deterministic and any observer may hide the problem.

**Data Race**: It is a type of *race condition* where memory operations in multiple threads attempt to access/modify the same location simultaneously. These code sections are referred as *critical sections*, they should be *mutually exclusive*. It is posible to have non-deterministic output even if the program is data-race free.

**Mutex**: Provides mutual exclusive access to shared data from multiple threads. However, adding a mutex lock to every function will result in no parallelism at all.

**Locks Bypassing**: Is dangerous and it is based on allowing other to keep a reference to the raw data. Then, we cannot guarantee proper locking, even if locks are used internally ([examples](src/section_2/02_mutex_misusages.cpp)):
1. If the container provides a function to get a reference or pointer to the resource.
2. If the container provides a function to run any code over the data.

**Race Condition Inherited from the Interface**: Having all methods guarded by a lock guarantees thread safety at the function execution level. However, race conditions can still happen if the user code depends on combinations of those functions. E.g., when `empty()` is used to later do `pop()` on a container ([example](src/section_2/03_thread_safe_stack.cpp)).

**Deadlock**: Is a state in which each thread is halted waiting for a lock which won't be released. It happens when multiple locks required, and they are not acquired in the same order. A deadlock without using locks!, by having threads attempting to call `join()` on the others. In the [example](src/section_2/03_thread_safe_stack.cpp), two scenarios arise:
1. The `transfer()` function attempts holding 2 locks for different accounts. But each account locks its own mutex first.
2. Different functions attempt holding 2 locks in different order.

**C++ Mutex and Guards**:
- The [std::mutex](https://en.cppreference.com/w/cpp/thread/mutex) primitive implements a mutex, for which we must call `lock()` and `unlock()`.
- The [std::lock_guard](https://en.cppreference.com/w/cpp/thread/lock_guard) is a RAII-style mutex wrapper for managing the lock automatically.
- The [std::scoped_lock](https://en.cppreference.com/w/cpp/thread/scoped_lock) is similar to `std::lock_guard`, but has a variadic constructor taking more than one mutex. This allows to lock multiple mutexes in a deadlock avoiding way as if `std::lock` were used. See also [stackoverflow.com](https://stackoverflow.com/questions/43019598/stdlock-guard-or-stdscoped-lock).
- The [std::unique_lock](https://en.cppreference.com/w/cpp/thread/unique_lock) is similar to `std::lock_guard`, but it does not have to acquire the lock during construction. It also allows time-constrained locking, recursive locking, conditional locking, and ownership transfer. In particular, the lock deferral allows acquiring multiple locks later using the `std::lock` function, as if `std::scoped_lock` were used.
- [mutex, lock_guard, and scoped_lock examples](src/section_2/01_mutex.cpp), [unique_lock examples](src/section_2/05_unique_lock.cpp).


### Condition Variables and Futures

**Condition Variables**:  [std::condition_variable](https://en.cppreference.com/w/cpp/thread/condition_variable) is a synchronization primitive used to block one or multiple threads until another thread modifies the variable and notifies to it:
- The notifier thread has to acquire a mutex to modify the variable (even if it is atomic). Then, it must execute `notify_one` or `notify_all` (no lock needed).
- Waiting threads must acquire the same mutex and:
  1. Check the condition. Execute `wait`, `wait_for`, or `wait_until`, atomically releasing the mutex. And awake on timeout, notification, or [spurious wakeup](https://en.wikipedia.org/wiki/Spurious_wakeup). Finally the condition must be checked to continue waiting or resume if needed.
  2. Or just use the predicated overload of `wait`, `wait_for`, and `wait_until`.
- For maximum efficiency, `std::condition_variable` works only with `std::unique_lock<std::mutex>`, while `std::condition_variable_any` works only with any lock.
- [example](src/section_3/01_condition_variable.cpp).

**Synchronous vs Asynchronous Operations**: A synchronous operation blocks a process until the operation completes (mutexes). An asynchronous operation is non-blocking and the caller should check for completion through another mechanism.

**Futures**: [std::future](https://en.cppreference.com/w/cpp/thread/future) is a mechanism to access the result of asynchronous operations. The asynchronous operation provides an `std::future` object to the creator, that can be used to query, wait, or extract the value. See [example](src/section_3/02_futures.cpp).

**Asynchronous Operation Creation**:
- [std::async](https://en.cppreference.com/w/cpp/thread/async): Allows running a function asynchronously, given a launch policy, function, and its arguments. Returns `std::future`. The [std::launch](https://en.cppreference.com/w/cpp/thread/launch) policy can be either `async` (new thread is created), `deferred` (lazy evaluation), or both of them (let the compiler decide). See: [example 1](src/section_3/03_async.cpp), [example 2](src/section_3/04_parallel_accumulate_async.cpp).
- [std::packaged_task](https://en.cppreference.com/w/cpp/thread/packaged_task): Wraps any callable target so that it can be invoked asynchronously. Returns value or exception on a `std::future` object. The invocation must be explicitely triggered. The wrapper can be moved to any thread, giving control over where it will execute. See: [example 1](src/section_3/05_packaged_task.cpp).
- [std::promise](https://en.cppreference.com/w/cpp/thread/promise): Is the *push* end of the promise-future communication channel for a shared state. The promise allows setting the ready value, releasing the reference, or abandon with exception. The promise is meant to be used only once. See the examples: [promise](src/section_3/06_promise.cpp), [promise exception](src/section_3/07_promise_exception.cpp).

**Shared Futures**: Once `get()` is called, the future object becomes invalid. Checking `valid()` is not enough, as a race condition exists. [std::shared_future](https://en.cppreference.com/w/cpp/thread/shared_future) is similar to `std::future`, but multiple threads are allowed to access the shared state. See the [example](src/section_3/08_shared_future.cpp).

### STL Containers and Algorithms

**STL and Thread Safety** [STL container](https://en.cppreference.com/w/cpp/container):
1. All container functions can be called concurrently by different threads on different containers.
2. All `const` member functions can be called concurrently by different threads on the same container. Operations like `begin()`, `end()`, `rbegin()`, `rend()`, `front()`, `back()`, `data()`, `find()`, `lower_bound()`, `upper_bound()`, `equal_range()`, `at()`, and, except in associative containers, `operator[]`, behave as const for the purposes of thread safety.
3. Different elements in the same container can be modified concurrently by different threads, except for the elements of `std::vector<bool>`.
4. Iterator operations (e.g. incrementing an iterator) read, but do not modify the underlying container, and may be executed concurrently with operations on other iterators on the same container, with the const member functions, or reads from the elements. Container operations that invalidate any iterators modify the container and cannot be executed concurrently with any operations on existing iterators even if those iterators are not invalidated.
5. Elements of the same container can be modified concurrently with those member functions that are not specified to access these elements.
6. In any case, container operations (or any other STL functions) may be parallelized internally as long as this does not change the user-visible results.

**Execution Policies**: C++17 added [execution policies](https://en.cppreference.com/w/cpp/algorithm/execution_policy_tag_t) for algorithms to specify that they should run sequentially, in parallel, o
r vectorized (sequentially, but with instructions that operate on multiple items).
- If the implementation cannot parallelize or vectorize, e.g. due to lack of resources, all policies can fall back to sequential execution.
- Some algorithms don't provide this feature, but the compiler can decide what is best. 
- The policy affects the algorithm complexity, behavior on exceptions, and the used algorithm implementation.
- The only exception that can be thrown is `std::bad_alloc`. Other unhandled exceptions will result in `std::terminate`.
- See: [sort example](src/section_4/01_execution_policies.cpp).

**Parallel STL-like algorithm examples**:
- [quicksort design](src/section_4/02_quicksort.cpp): Parallel divide and conquer.
- [for_each design](src/section_4/03_foreach.cpp): Parallel divide-and-conquer and parallel `packaged_task`.
- [find](src/section_4/04_find.cpp): Early termination when found.

**Factors Affecting the Performance of Concurrent Code**:
- **Number of processors**: Using more threads than available processors leads to oversubscription and excessive task switching. Relying on `hardware_concurrency()` is not enough, as the application could launch threads we dont know about. `std::async` has *application level visibility of the number of threads launched by the application*, and thus, it automatically decides when to launch a thread or defer. If multiple multi-threaded application are running on the machine, it is best to use a global observer on the number of running threads.
- **Data contention and cache ping pong**: Assuming each thread runs on a different core, the CPU is going to maintain a cache over the data for each thread. If both threads operate over the same data, whenever 1 thread performs write operations, the cache for the other threads needs to be updated! This is very slow. Even worse, the situation will repeat on every update.
- **False Sharing**: Processor caches usually work by memory blocks (32 or 64 bytes) called *cache lines*. The line will be maintained even when we only acess to 8 bytes. *False sharing* happens when each thread operates on different variables, but these are kept under the same cache line, thus, leading to data contention.
- **Closeness of the data**: When the data is sparse, a single thread will have to load multiple cache lines. This leads to more load/update operations and there wil be more uninteresting data in the cache.

### C++20 Addons

**JThread**: [std::jthread](https://en.cppreference.com/w/cpp/thread/jthread) has the same general behavior as `std::thread`, but it automatically rejoins on destruction (RAII) and can be cancelled/stopped on certain situations. See the [example](src/section_5/01_jthread.cpp).
- The [std::stop_token](https://en.cppreference.com/w/cpp/thread/stop_token) can be added as a function argument in order to introduce interrupt points in the thread. This argument will be provided by the `jthread` constructor.
- The [request_stop()](https://en.cppreference.com/w/cpp/thread/jthread/request_stop) method issues a stop request to the jthread. If no `stop_token` is used, then the request is ignored.
- The destructor [~jthread()](https://en.cppreference.com/w/cpp/thread/jthread/%7Ejthread) calls `request_stop()` and then `join()`.

**Coroutines**: [coroutines](https://en.cppreference.com/w/cpp/language/coroutines). Coroutines are functions which can be suspended and resumed. See the [example](src/section_5/02_coroutines.cpp).
- **Subroutines and Coroutines**:
  - **Subroutines**: Refer to normal functions and work on two stages, invocation and finalization. During invocation they push their code into the execution stack. Finalization is triggered when the function goes out of scope. and the stack is cleared. The callers are blocked until finalization.
  - **Coroutines**: They add 2 more stages, suspend and resume. The coroutine can opt to suspend its own execution, and the owner of the handler can resume it. Whenever a function contains the keywords `co_await`, `co_yield`, or `co_return`, the compiler considers it as a coroutine.
- **Heap and Stack**: Coroutines are created in the heap and in suspended state. When the execution resumes, their core is copied into the caller stack. When the coroutine suspends, the heap object is updated using the values from the stack. Coroutines do not use the stack by themselves, but only to copy-from and update the heap.
- **Components**: A coroutine consists of 3 parts: A promise, a handle, and coroutine state.
  1. The `promise_type`: is user defined, its manipulated from inside the coroutine, and it allows returning the result. It is not related to `std::promise`!.
  2. The handle: is non-owning and it is used to resume or destroy the coroutine from outside.
  3. The state: is typically heap allocated, contains the promise object, arguments, and local variables.
- **Generators**: Coroutines can be used to implement lazy generators. These are based on the `co_yield` keyword. See [example](src/section_05/03_generators.cpp).

**Barriers and Latches**: 
- [std::barrier](https://en.cppreference.com/w/cpp/thread/barrier) is a synchronization mechanism that forces all threads to wait until all of them reach certain point in code. Barriers are reusable.
- [std::latch](https://en.cppreference.com/w/cpp/thread/latch) is similar to `std::barrier`, but it has a single use.
- Both `std::barrier` and `std::latch` are provided by the `<barrier>` and `<latch>` headers. These are not supported in gcc-10. However, `<boost/thread/barrier>` can be used as a replacement.


### Memory Model and Atomic Operations

**Atomic Operation:** Indivisible operation. It cannot be observed half-done from any thread in the system. If one thread writes to an atomic object while another thread reads from it, the behavior is well-defined.

**Lock-Free vs. Lock-Based**: Atomics allow writting lock-free multithreading, meaning there is no need to use any synchronization primitive.

**Atomic Type**: Refers to a type where all operations on it are considered atomic. The [std::atomic](https://en.cppreference.com/w/cpp/atomic/atomic) template class allows defining atomic types and C++ provides several atomics for primitive types.
- Accesses to `std::atomic` objects may establish inter-thread synchronization and order non-atomic memory accesses as specified by `std::memory_order`.
- `std::atomic<>` is copy- and move- disabled. However, atomics can be set and constructed using variables from the non-atomic type. In particular, it can be instantiated with any *TriviallyCopyable* type, that satisfies *CopyConstructible* and *CopyAssignable*.

**Relevant std::atomic functions:**
- `is_lock_free()`: Check if compiler added internal locks. Depends on OS.
- `store(T), load()`: Write new value based on atomic or non-atomic variable. Read current value. Using these functions instead of implicit read/write is preferred for cognitive purposes.
- `exchange(T)`: Atomically replace the value with a new one and return the old one (atomic swap).
- [compare_exchange](https://en.cppreference.com/w/cpp/atomic/atomic/compare_exchange): Is a conditional swap. It is also called *Compare-and-Swap* (CAS):
- Given the signature `compare_exchange_*(T& expected, T desired)`, where `expected` refers to the value we expect to see when calling `load()`, and `desired` refers to the new value we would like to set. If `current_value == expected`, sets `current_value = desired` and returns `true`.  Otherwise, sets `expected = current_value` and returns `false`. In other words, if the expectation matches, performs an update. Otherwise, signals the caller that the expectation failed.
  - `compare_exchange_weak()`: Does not guarantees operation to be successful, even if they expected is equal to the current value. It may happen when the machine does not have the *compare_exchange* instruction (like in RISC processors used in ARM). However, this is usually faster to execute and the operation could be repeated.
  - `compare_exchange_strong()`: Guarantees to be atomic and successful, but it may use more than one instruction.
  - Compare-and-exchange operations are often used as basic building blocks of lockfree algorithms and data structures. These are based on while statements like: `std::atomic<int> x{0}; int x0 = x; while(!x.compare_exchange_strong(x0, x0+1)) {}`, `x0` will have a proper value after completion, even on multithreading apps. Notice how the CAS allows lock-free multithreading.
  - TODO: WHY IS THIS FUNCTION SO IMPORTANT?

**Relevant Atomic Types**:
- [atomic_flag](https://en.cppreference.com/w/cpp/atomic/atomic_flag): Atomic boolean type guaranteed to be lock free. All other types provide `is_lock_free()` (value depends on the OS). The API is limited compared to `std::atomic<bool>`.
- `std::atomic<U*>`: Atomic pointer. The pointed object is not atomic. Also support atomic arithmetic operations: `fetch_add`, `+=`, `fetch_sub`, `-=`, `++`, `--`. `std::atomic<std::shared_ptr<U>>` and `std::atomic<std::weak_ptr<U>>` are also available.

**Atomics and False Sharing**: Atomics are also affected by the false sharing issue, specially for writting. They have to wait for cache line access. This can be avoided by aligning per-thread data to separate cache lines, or even multiple pages.

**Atomics as gateways to Memory Access**:
- Atomics are used to get exclusive access to memory or to real memory to other threads. They can be used as generalized pointers!. See the [example](src/section_6/01_atomic.cpp) on atomic structures:
  1. Acquire Exclusive Access: `std::atomic<T>` as an array index can be used as a *pointer to variable*. Using `fetch_add` we can get exclusive access to an index (memory location) which other threads won't be able to access.
  2. Release into Shared Access: `std::atomic<U*>` can be used as a generalized pointer, allowing swapping the pointer to another memory location atomically. Thus, revealing new memory to other threads (e.g. update head of linked list).
- Most memory is not atomic!, but atomics are used as handlers to avoid race conditions.

**Memory Model**: In todays processors, memory is layered into core registers, L1 cache, store buffers, L2 cache, L3 cache, and main memory. Registers and L1 cache are processor specific, next layers are shared between groups of cores, and the main memory is shared between all the cores.
- Memory operations (read/write) are propagated between the cores and the memory; *Write* operations are propagated from the core up to the main memory, and *read* operations are propagated from the memory down to the core. Sometimes the propagation is delayed, it may only reach an internal layer, or even the operation ordering may be switched!.
- The source code may not reflect what really happens in the hardware:
  1. Compilers may perform subexpression elimination and allocate registers.
  2. Caches may use *store buffers*, or may be shared between some of the cores.
  3. Processors may perform prefetch, speculate, or overlap writes.
- Both mutexes and memory barriers are a way to impose restrictions on what alterations may happen.
- Imposing ordering restrictions into the operations, may result into additional `store` operations, additional cache refresh, and it may even restrict the instruction reordering for optimizations.

**Memory Barriers**: They control how changes to memory made by one CPU become visible to other CPUs. The visibility refers to the moment when a desired memory layer is set with up-to-date information. Understanding memory barriers is essential to understand atomics and memory guarantees.
- They are important as, by default, there is no guarantee of visibility. Threads could just keep modifying their own caches and never be aware of the updates. Visibility of non-atomic changes is not guaranteed.
- They are a global control of visibility across all CPUs. Synchronization of data access is not possible if we cannot control the order of memory access.
- They are implemented by the hardware. They are invoked through processor-specific instructions.
- They are often attributes on atomic read/write operations, ensuring specified order of reads and writes.

**Available Memory Barriers** (see [std::memory_order](https://en.cppreference.com/w/cpp/atomic/memory_order)):
  - `std::memory_order_relaxed`: No memory barrier. There is no guaranteed ordering constraints imposed on other reads and writes. Only this operation atomicity is guaranteed.
  - `std::memory_order_acquire`: The *half-barrier*. Guarantees that all memory operations scheduled after the barrier, including operations on other variables, become visible after the barrier. Reads/Writes cannot be reordered before the barrier. However, operations before the barrier can be reordered or even moved after the barrier. Only valid for the thread that issued the barrier.
  - `std::memory_order_release`: Reverse of the acquire barrier. Nothing that was before can be observed after the barrier. But operations after the barrier may be reordered before it. It guarantees all operations before it become visible.
  - `std::memory_order_acq_rel`: Combines both. No operation can move across the barrier. But only if both threads use the same atomic variable!.
  - `std::memory_order_seq_cst` (default): The Sequential Consistency is the most strict barrier. It forces `load` operations to performs acquire, `store` operations to perform release, and read-modify-write to perform both. If enforces a single total order in which all threads observe all modifications in the same order. The behavior of the program is consistent with a simple sequential view of the code.
  - `std::memory_order_consume`: Special case of acquire that limits the synchronization of data to direct dependencies. The *carries-a-dependency-to* relationship applies within a thread, and happens when the result of an operation is used as an operand for the atomic variable of interest.
  
**Memory Barrier Protocols**: See the [example](src/section_6/02_memory_barriers.cpp). More patterns can be found in the [official documentation](https://en.cppreference.com/w/cpp/atomic/memory_order).
- **Release-Acquire Protocol**: Thread 1 writes data on atomic variable (releases) using the release barrier, while thread 2 reads data from the same variable (acquires) using the acquire barrier. There is a guarantee than the released atomic (and other variables set before it) is visible when acquired.
- **Release-Consume Protocol**: Similar to the protocol above, but the acquire barrier is relaxed to only restrict dependencies.

**Memory Barrier Considerations**:
- The strongest memory order *may* be too expensive in terms of time and cognitive load!:
  - Sequential consistency makes the program easier to read and often has no performance penalty!.
  - Making every operation `seq_cst` is not necessary and obscures programmer's intent.
  - Use memory order to express programmer's intent.
- Not all platforms provide all barriers.
- In some platforms, some barriers are cheap: On x86, all loads are acquire (for free) and all stores are release (for free), but trying to use load-release and store-acquire is expensive. Also, there is no difference between `acq_rel` and `seq_cst`.
- Use the right barrier!.

**Transitive Synchronization**: It allows synchronizing 3 threads without having any release/acquire ordering mechanism. If threads `A->B` (A is synchronized with B, where commands in A happen before the ones in B) and `B->C`, then `A->C`. See the [example](src/section_6/03_transitive_synchronization.cpp).

**When to use std::atomic**:
- High-performance concurrent lock-free data structures (benchmark it!).
- Data structures that are difficult or expensive to implement with locks (lists, trees).
- When lock problems are important: deadlocks, priority, latency.
- When concurrent synchronization can be achieved by the cheapest atomic operations: store and load.

### Lock Free Data Structures And Algorithms




### TODOs

- Look for TODOs
- C++20: [std::counting_semaphore](https://en.cppreference.com/w/cpp/thread/counting_semaphore)
- Memory Model: [memory_model](https://en.cppreference.com/w/cpp/language/memory_model) 

```c++
// =============================================================================
// Example:
// =============================================================================

// TODO: Memory locations:
// See: https://en.cppreference.com/w/cpp/language/memory_model
struct S {
  char a;     // location #1: pos 0
  int b : 5;  // location #2: pos 5
  int c : 11, // location #2:
      : 0,    //
      d : 8;
};
```
