project(section_4)

add_executable(01_execution_policies 01_execution_policies.cpp)
target_link_libraries(01_execution_policies pthread)

add_executable(02_quicksort 02_quicksort.cpp)
target_link_libraries(02_quicksort pthread)

add_executable(03_foreach 03_foreach.cpp)
target_link_libraries(03_foreach pthread)

add_executable(04_find 04_find.cpp)
target_link_libraries(04_find pthread)
