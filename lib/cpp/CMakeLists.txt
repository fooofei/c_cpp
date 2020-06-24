cmake_minimum_required(VERSION 3.1)

project(tests)

if(APPLE)
    # no warning
    # https://stackoverflow.com/questions/31561309/cmake-warnings-under-os-x-macosx-rpath-is-not-specified-for-the-following-targe
    set(CMAKE_MACOSX_RPATH 0)
endif()



add_executable(test_file_iterator_usn
  ${CMAKE_CURRENT_SOURCE_DIR}/file_iterator_usn.h
  ${CMAKE_CURRENT_SOURCE_DIR}/file_iterator_usn.cpp
)
if(WIN32)
	target_compile_options(test_file_iterator_usn PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()