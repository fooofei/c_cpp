cmake_minimum_required(VERSION 2.6)
project(call)

if (APPLE)
    set(CMAKE_MACOSX_RPATH 0)
endif ()


set(Source_files executable.cpp)

set(COMMON_HOME ${CMAKE_CURRENT_SOURCE_DIR}/../../../)

if(WIN32)
 list(APPEND Source_files 
	${COMMON_HOME}/crt_dbg_leak.h
	${COMMON_HOME}/crt_dbg_leak.c
	${COMMON_HOME}/crt_dbg_leak.cpp
 )
endif()


add_executable(${PROJECT_NAME} ${Source_files})

target_include_directories(${PROJECT_NAME} PRIVATE ${COMMON_HOME})

if (NOT WIN32)
    target_compile_options(${PROJECT_NAME} PRIVATE -fvisibility=hidden)
endif ()



if (NOT TARGET print_chs)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../print_chs build_print_chs)
endif ()
target_link_libraries(${PROJECT_NAME} print_chs)

if (NOT TARGET whereami)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../whereami build_whereami)
endif ()
target_link_libraries(${PROJECT_NAME} whereami)


if (NOT TARGET library_loader)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../../library_loader build_library_loader)
endif ()
target_link_libraries(${PROJECT_NAME} library_loader)

if(WIN32)
	target_compile_options(${PROJECT_NAME} PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()



add_custom_command(TARGET ${PROJECT_NAME}
        POST_BUILD
        # Cann't use mv, it not work on Windows.
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ../../bin/
        # COMMAND ${CMAKE_COMMAND} -E remove $<TARGET_FILE:${PROJECT_NAME}>
        )
