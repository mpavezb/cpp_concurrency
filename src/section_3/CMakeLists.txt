project(section_3)

add_executable(01_condition_variable 01_condition_variable.cpp)
target_link_libraries(01_condition_variable pthread)

add_executable(02_futures 02_futures.cpp)
target_link_libraries(02_futures pthread)
