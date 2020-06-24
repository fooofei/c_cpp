# This CMakeLists.txt is for encoding module.
# Other Module can use it by this:
#
# if (NOT TARGET print_chs)
# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../print_chs build_print_chs)
# endif ()
# target_link_libraries(${PROJECT_NAME} print_chs)
#
#
# add_subdirectory() is add this module for build.
# first argument is to tell the module(CMakeLists.txt) location,
# second argument is to tell where to build the module, make sure not to use
# the same directory with the current module.
#
# target_link_libraries() tell to link the module.
#
#

cmake_minimum_required(VERSION 2.8.14)
project(print_chs)

if (APPLE)
    set(CMAKE_MACOSX_RPATH 0)
endif ()


add_library(${PROJECT_NAME} STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/print_chs.cpp)

# for others module include
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)


if (NOT WIN32)
    # fPIC for other library link
    target_compile_options(${PROJECT_NAME} PRIVATE -fPIC)
endif()
