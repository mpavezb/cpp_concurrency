project(section_5)

add_executable(01_jthread 01_jthread.cpp)
target_link_libraries(01_jthread pthread)

add_executable(02_coroutines 02_coroutines.cpp)
target_link_libraries(02_coroutines pthread)

add_executable(03_generators 03_generators.cpp)
target_link_libraries(03_generators pthread)
