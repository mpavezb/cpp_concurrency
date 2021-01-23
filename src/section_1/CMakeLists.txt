project(section_1)

add_executable(exercise_1 exercise_1.cpp)
target_link_libraries(exercise_1 pthread)

add_executable(exercise_2 exercise_2.cpp)
target_link_libraries(exercise_2 pthread)

add_executable(01_joinability 01_joinability.cpp)
target_link_libraries(01_joinability pthread)

add_executable(02_detach 02_detach.cpp)
target_link_libraries(02_detach pthread)

add_executable(03_exceptions 03_exceptions.cpp)
target_link_libraries(03_exceptions pthread)

add_executable(04_thread_parameters 04_thread_parameters.cpp)
target_link_libraries(04_thread_parameters pthread)

add_executable(05_pass_by_ref_and_detach 05_pass_by_ref_and_detach.cpp)
target_link_libraries(05_pass_by_ref_and_detach pthread)

add_executable(06_ownership 06_ownership.cpp)
target_link_libraries(06_ownership pthread)

add_executable(07_useful_api 07_useful_api.cpp)
target_link_libraries(07_useful_api pthread)
