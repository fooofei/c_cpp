# This CMakeLists.txt is for encoding module.
# Other Module can use it by this:
#
# if (NOT TARGET xxx)
# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../xxx  build_xxx)
# endif ()
# target_link_libraries(${BINARY_NAME} xxx)
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
project(library_loader)

set(BINARY_NAME ${PROJECT_NAME})



set(Source_files_files
        ${CMAKE_CURRENT_SOURCE_DIR}/library_loader.h
        ${CMAKE_CURRENT_SOURCE_DIR}/library_loader.c
        )

add_library(${BINARY_NAME} STATIC ${Source_files_files})

# for others module include
target_include_directories(${BINARY_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

if (NOT WIN32)
  # fPIC for other library link
  target_compile_options(${BINARY_NAME} PRIVATE -fPIC)
  target_link_libraries(${BINARY_NAME} dl)
  target_compile_options(${BINARY_NAME} PRIVATE -fvisibility=hidden)
endif()



source_group(files FILES ${Source_files_files})

#add_custom_command(TARGET ${PROJECT_NAME}
#        POST_BUILD
#        COMMAND mv $<TARGET_FILE_NAME:${PROJECT_NAME}> ../
#        )



if (APPLE)
  set(CMAKE_MACOSX_RPATH 0)
endif ()