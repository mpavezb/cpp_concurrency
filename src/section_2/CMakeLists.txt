project(section_2)

add_executable(01_mutex 01_mutex.cpp)
target_link_libraries(01_mutex pthread)

add_executable(02_mutex_misusages 02_mutex_misusages.cpp)
target_link_libraries(02_mutex_misusages pthread)

add_executable(03_thread_safe_stack 03_thread_safe_stack.cpp)
target_link_libraries(03_thread_safe_stack pthread)

add_executable(04_deadlock 04_deadlock.cpp)
target_link_libraries(04_deadlock pthread)

add_executable(05_unique_lock 05_unique_lock.cpp)
target_link_libraries(05_unique_lock pthread)
