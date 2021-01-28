project(section_6)

add_executable(01_atomic 01_atomic.cpp)
target_link_libraries(01_atomic pthread)

add_executable(02_memory_barriers 02_memory_barriers.cpp)
target_link_libraries(02_memory_barriers pthread)
