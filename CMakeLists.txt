cmake_minimum_required(VERSION 3.1)

project(tests)

#set(SOURCES cxx11_thread_mutex.cpp)
#set(SOURCES kmp_prefix_array1.cpp)

add_executable(test_case 
    ${CMAKE_CURRENT_SOURCE_DIR}/cast_pointer_2_uint32_nowarning.c
)
if(WIN32)
	target_compile_options(test_case PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()

add_executable(test_sort
    ${CMAKE_CURRENT_SOURCE_DIR}/sort.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sort.c
)
if(WIN32)
	target_compile_options(test_sort PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()

add_executable(test_search
    ${CMAKE_CURRENT_SOURCE_DIR}/search.h
    ${CMAKE_CURRENT_SOURCE_DIR}/search.c
)
if(WIN32)
	target_compile_options(test_search PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()


add_executable(test_sqrt
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp_sqrt.c
)
if(WIN32)
	target_compile_options(test_sqrt PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()


add_executable(test_cxx11_thread_mutex
    ${CMAKE_CURRENT_SOURCE_DIR}/cxx11_thread_mutex.cpp
)
if(WIN32)
	target_compile_options(test_cxx11_thread_mutex PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()

add_executable(test_file_operation
    ${CMAKE_CURRENT_SOURCE_DIR}/file_operation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/file_operation.h
)
if(WIN32)
	target_compile_options(test_file_operation PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()

add_executable(test_hex_strs
    ${CMAKE_CURRENT_SOURCE_DIR}/hex_strs.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hex_strs.h
)
if(WIN32)
	target_compile_options(test_hex_strs PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()


add_executable(test_memory_buffer_as_file
    ${CMAKE_CURRENT_SOURCE_DIR}/memory_buffer_as_FILE.cpp
)

add_executable(test_ostringstream_sprintf
    ${CMAKE_CURRENT_SOURCE_DIR}/ostringstream_sprintf.cpp
)


add_executable(test_pimpl
    ${CMAKE_CURRENT_SOURCE_DIR}/pimpl.cpp
)


add_executable(test_struct_embeded_c
    ${CMAKE_CURRENT_SOURCE_DIR}/struct_embeded.c
)
add_executable(test_struct_embeded_cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/struct_embeded.cpp
)


add_executable(test_top_k_heap
    ${CMAKE_CURRENT_SOURCE_DIR}/top_k_heap.cpp
)
if(WIN32)
	target_compile_options(test_top_k_heap PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()



add_executable(test_std_binary_search
    ${CMAKE_CURRENT_SOURCE_DIR}/std_binary_search_test_case.cpp
)
if(WIN32)
	target_compile_options(test_std_binary_search PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()

if(APPLE)
    # no warning
    # https://stackoverflow.com/questions/31561309/cmake-warnings-under-os-x-macosx-rpath-is-not-specified-for-the-following-targe
    set(CMAKE_MACOSX_RPATH 0)
endif()
