cmake_minimum_required(VERSION 2.6)
project(whereamitestlib)

if (APPLE)
    set(CMAKE_MACOSX_RPATH 0)
endif ()


add_library(${PROJECT_NAME} SHARED library.cpp)

if (NOT WIN32)
    target_compile_options(${PROJECT_NAME} PRIVATE -fvisibility=hidden)
endif()

# 第二个参数设置上级目录 ${CMAKE_HOME_DIRECTORY}/build_encoding 总是不好
# error : make[1]: *** No rule to make target '../build_encoding/CMakeFiles/encoding.dir/all'

if (NOT TARGET print_chs)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../print_chs build_print_chs)
endif ()
target_link_libraries(${PROJECT_NAME} print_chs)

if (NOT TARGET whereami)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../whereami build_whereami)
endif ()
target_link_libraries(${PROJECT_NAME} whereami)

add_custom_command(TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ../../bin/
        COMMAND ${CMAKE_COMMAND} -E remove $<TARGET_FILE:${PROJECT_NAME}>
        )
