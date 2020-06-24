cmake_minimum_required (VERSION 2.6)
project (proj)


if (POLICY CMP0042)
    # Newer cmake on MacOS should use @rpath
    cmake_policy (SET CMP0042 NEW)
endif ()

set(BINARY_NAME_PLUGIN1 plugin1)


set(Source_files 
	${CMAKE_CURRENT_SOURCE_DIR}/plugin1.c
	${CMAKE_CURRENT_SOURCE_DIR}/../api.h
	)


add_library(${BINARY_NAME_PLUGIN1} SHARED ${Source_files})

source_group(files FILES ${Source_files})



if(WIN32)
	target_compile_options(${BINARY_NAME_PLUGIN1} PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()

add_custom_command(
        TARGET ${BINARY_NAME_PLUGIN1} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "[status] copy to" ${CMAKE_CURRENT_SOURCE_DIR}/../bin/$<TARGET_FILE_NAME:${BINARY_NAME_PLUGIN1}>
        COMMAND ${CMAKE_COMMAND} -E copy  $<TARGET_FILE:${BINARY_NAME_PLUGIN1}> ${CMAKE_CURRENT_SOURCE_DIR}/../bin/$<TARGET_FILE_NAME:${BINARY_NAME_PLUGIN1}>
)



# also waring
