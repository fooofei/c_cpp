
cmake_minimum_required(VERSION 3.1)
project(C_MAKE_DEMO)

if(APPLE)
set(CMAKE_MACOSX_RPATH 0)
endif()


set(BINARY_NAME test_core_dump)

set(Source_files 
	${CMAKE_CURRENT_SOURCE_DIR}/coredump.c
	${CMAKE_CURRENT_SOURCE_DIR}/coredump.h

	)

add_executable(${BINARY_NAME} ${Source_files})

source_group(files FILES ${Source_files})

if(WIN32)
	target_compile_options(${BINARY_NAME} PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()