cmake_minimum_required(VERSION 3.1)
project(proj)

add_subdirectory(plugin1)

set(BINARY_NAME_LOADER loader_plugin_tests)

set(Source_files 
	${CMAKE_CURRENT_SOURCE_DIR}/loader.c
	${CMAKE_CURRENT_SOURCE_DIR}/api.h

	)

add_executable(${BINARY_NAME_LOADER} ${Source_files})

source_group(files FILES ${Source_files})


if (NOT TARGET library_loader)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/..  library_loader)
endif ()
target_link_libraries(${BINARY_NAME_LOADER} library_loader)


if(WIN32)
	target_compile_options(${BINARY_NAME_LOADER} PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()
if(APPLE)
    # no warning
    # https://stackoverflow.com/questions/31561309/cmake-warnings-under-os-x-macosx-rpath-is-not-specified-for-the-following-targe
    set(CMAKE_MACOSX_RPATH 0)
endif()


add_custom_command(
        TARGET ${BINARY_NAME_LOADER} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E echo "[status] copy to" ${CMAKE_CURRENT_SOURCE_DIR}/bin/$<TARGET_FILE_NAME:${BINARY_NAME_LOADER}>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${BINARY_NAME_LOADER}> ${CMAKE_CURRENT_SOURCE_DIR}/bin/$<TARGET_FILE_NAME:${BINARY_NAME_LOADER}>
)