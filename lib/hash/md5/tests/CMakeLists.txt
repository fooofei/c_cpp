

cmake_minimum_required(VERSION 2.8.14)
project(test_md5)


set(BINARY_NAME test_md5)

set(Source_files 
  ${CMAKE_CURRENT_SOURCE_DIR}/main.c
  ${CMAKE_CURRENT_SOURCE_DIR}/../md5.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../md5.c
)

add_executable(${BINARY_NAME} ${Source_files})


if(WIN32)
 # no utf-8 warning in Visual Studio https://github.com/fooofei/cpp_notes/blob/master/visual_sutdio_warning_C4819_utf-8.md
 target_compile_options(${BINARY_NAME} PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()

if (APPLE)
    set(CMAKE_MACOSX_RPATH 0)
endif ()