cmake_minimum_required(VERSION 3.1)

project(test_demo)

#set(SOURCES cxx11_thread_mutex.cpp)
#set(SOURCES kmp_prefix_array1.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

#set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 11 )