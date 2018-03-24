cmake_minimum_required(VERSION 3.1)

project(tests)

if(APPLE)
    # no warning
    # https://stackoverflow.com/questions/31561309/cmake-warnings-under-os-x-macosx-rpath-is-not-specified-for-the-following-targe
    set(CMAKE_MACOSX_RPATH 0)
endif()


foreach(i ${CMAKE_CXX_COMPILE_FEATURES})
  message(STATUS "compiler support ${i}")
endforeach()



add_executable(test_cast
    ${CMAKE_CURRENT_LIST_DIR}/cast_pointer_2_uint32_nowarning.c
)
if(WIN32)
	target_compile_options(test_cast PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()





add_executable(test_sqrt
    ${CMAKE_CURRENT_LIST_DIR}/cpp_sqrt.c
)
if(WIN32)
	target_compile_options(test_sqrt PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()




# cxx11 c11
if(WIN32)
add_executable(test_cxx11_thread_mutex
    ${CMAKE_CURRENT_LIST_DIR}/cxx11_thread_mutex.cpp
    ${CMAKE_CURRENT_LIST_DIR}/c_cpp_common/crt_dbg_leak.cpp
    ${CMAKE_CURRENT_LIST_DIR}/c_cpp_common/crt_dbg_leak.c
    ${CMAKE_CURRENT_LIST_DIR}/c_cpp_common/crt_dbg_leak.h
)
if(WIN32)
	target_compile_options(test_cxx11_thread_mutex PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()
# too late
# set_property(TARGET test_cxx11_thread_mutex PROPERTY CXX_STANDARD 11)
endif()




# Windows only 
if(WIN32)
	add_executable(test_file_operation
		${CMAKE_CURRENT_LIST_DIR}/file_operation.cpp
		${CMAKE_CURRENT_LIST_DIR}/file_operation.h
	)
	if(WIN32)
		target_compile_options(test_file_operation PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
	endif()
endif()

add_executable(test_hex_strs
    ${CMAKE_CURRENT_LIST_DIR}/hex_strs.cpp
    ${CMAKE_CURRENT_LIST_DIR}/hex_strs.h
)
if(WIN32)
	target_compile_options(test_hex_strs PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()



add_executable(test_ostringstream_sprintf
    ${CMAKE_CURRENT_LIST_DIR}/ostringstream_sprintf.cpp
)


add_executable(test_pimpl
    ${CMAKE_CURRENT_LIST_DIR}/pimpl.cpp
)


add_executable(test_struct_embeded_c
    ${CMAKE_CURRENT_LIST_DIR}/struct_embeded.c
)
add_executable(test_struct_embeded_cpp
    ${CMAKE_CURRENT_LIST_DIR}/struct_embeded.cpp
)


add_executable(test_top_k_heap
    ${CMAKE_CURRENT_LIST_DIR}/top_k_heap.c
)
if(WIN32)
	target_compile_options(test_top_k_heap PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()



add_executable(test_std_binary_search
    ${CMAKE_CURRENT_LIST_DIR}/std_binary_search_test_case.cpp
)
if(WIN32)
	target_compile_options(test_std_binary_search PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()


add_executable(test_flexible_array 
 ${CMAKE_CURRENT_LIST_DIR}/flexible_array.c
)
if(WIN32)
	target_compile_options(test_flexible_array PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()



add_executable(test_setlocal_free_or_not
 ${CMAKE_CURRENT_LIST_DIR}/setlocal_free_or_not.c
)




add_executable(test_stack_queue_min_value
 ${CMAKE_CURRENT_LIST_DIR}/stack_queue_min_value.cpp
)





#cxx11 c11
if(WIN32)
add_executable(test_on_scope_exit
   ${CMAKE_CURRENT_LIST_DIR}/on_scope_exit.cpp
)
endif()



add_executable(test_new_operator_and_operator_new_1
   ${CMAKE_CURRENT_LIST_DIR}/new_operator_and_operator_new_1.cpp
)
if(WIN32)
	target_compile_options(test_new_operator_and_operator_new_1 PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()

add_executable(test_new_operator_and_operator_new_2
   ${CMAKE_CURRENT_LIST_DIR}/new_operator_and_operator_new_2.cpp
)
if(WIN32)
	target_compile_options(test_new_operator_and_operator_new_2 PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()

add_executable(test_new_operator_and_operator_new_3
   ${CMAKE_CURRENT_LIST_DIR}/new_operator_and_operator_new_3.cpp
)
if(WIN32)
	target_compile_options(test_new_operator_and_operator_new_3 PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()

add_executable(test_new_operator_and_operator_new_4
   ${CMAKE_CURRENT_LIST_DIR}/new_operator_and_operator_new_4.cpp
)
if(WIN32)
	target_compile_options(test_new_operator_and_operator_new_4 PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()

add_executable(test_new_operator_and_operator_new_5
   ${CMAKE_CURRENT_LIST_DIR}/new_operator_and_operator_new_5.cpp
)
if(WIN32)
	target_compile_options(test_new_operator_and_operator_new_5 PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()

add_executable(test_new_operator_and_operator_new_6
   ${CMAKE_CURRENT_LIST_DIR}/new_operator_and_operator_new_6.cpp
)
if(WIN32)
	target_compile_options(test_new_operator_and_operator_new_6 PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()

add_executable(test_new_operator_and_operator_new_7
   ${CMAKE_CURRENT_LIST_DIR}/new_operator_and_operator_new_7.cpp
)
if(WIN32)
	target_compile_options(test_new_operator_and_operator_new_7 PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()
if(WIN32)
	target_compile_options(test_new_operator_and_operator_new_7 PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()




add_executable(test_merge_m_sorted
${CMAKE_CURRENT_LIST_DIR}/merge_m_sorted.cpp
)



# Windows only
if(WIN32)
add_executable(test_make_file_untrust
${CMAKE_CURRENT_LIST_DIR}/make_file_untrust.cpp
)
endif()


add_executable(test_kmp_prefix_array
 ${CMAKE_CURRENT_LIST_DIR}/kmp_prefix_array.cpp
)



# cxx11 c11
if(WIN32)
add_executable(test_is_there_a_loop_by_stackoverflow
 ${CMAKE_CURRENT_LIST_DIR}/is_there_a_loop_by_stackoverflow.cpp
)
endif()



add_executable(test_howto_typedef_array
 ${CMAKE_CURRENT_LIST_DIR}/howto_typedef_array_1_error.cpp
 ${CMAKE_CURRENT_LIST_DIR}/howto_typedef_array_2_right.cpp
)


if(WIN32)
add_executable(test_cache_coherency
${CMAKE_CURRENT_LIST_DIR}/test_cache_coherency.cpp
)
set_target_properties(test_cache_coherency PROPERTIES  CXX_STANDARD 11)
endif()

if(WIN32)
	target_compile_options(test_cache_coherency PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()


if(NOT WIN32)
    add_executable(test_pthread_cond 
    ${CMAKE_CURRENT_LIST_DIR}/pthread_cond_t.c
    )
    target_link_libraries(test_pthread_cond pthread)  
endif()

if(NOT WIN32)
    add_executable(test_pthread_cond2 
    ${CMAKE_CURRENT_LIST_DIR}/pthread_cond_t2.c
    )
    target_link_libraries(test_pthread_cond2 pthread)  
endif()




 add_executable(test_array_out_bound
    ${CMAKE_CURRENT_LIST_DIR}/array_out_bound.c
    )



 add_executable(test_binary_tree_no_recursion_free
    ${CMAKE_CURRENT_LIST_DIR}/binary_tree_no_recursion_free.c
    )