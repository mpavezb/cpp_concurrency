project(src) # can this be deleted?

add_executable(0_thread 0_thread.cpp)
target_link_libraries(0_thread pthread)

add_subdirectory(section_1)
add_subdirectory(section_2)
