project(src) # can this be deleted?

#find_package(Boost REQUIRED)
#include_directories(${Boost_INCLUDE_DIRS})

add_executable(0_thread 0_thread.cpp)
target_link_libraries(0_thread pthread)
