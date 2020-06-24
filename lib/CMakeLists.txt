cmake_minimum_required(VERSION 3.1)

project(tests)

if(APPLE)
    set(CMAKE_MACOSX_RPATH 0)
endif()

# target_compile_options(${self_binary_name} PRIVATE -fPIC)
# target_compile_options(${self_binary_name} PRIVATE -fvisibility=hidden)
# target_compile_options(${self_binary_name} PRIVATE -fno-common )

add_executable(test_fio
    ${CMAKE_CURRENT_SOURCE_DIR}/fio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/fio.h
)
if(WIN32)
	target_compile_options(test_fio PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()






add_executable(test_datetime
    ${CMAKE_CURRENT_SOURCE_DIR}/datetime.c
    ${CMAKE_CURRENT_SOURCE_DIR}/datetime.h
    ${CMAKE_CURRENT_SOURCE_DIR}/datetime.cpp
)




add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/library_loader/tests)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/hash/sha1/tests)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/hash/md5/tests)





set(string_source_files 
	${CMAKE_CURRENT_SOURCE_DIR}/cstring.c
	${CMAKE_CURRENT_SOURCE_DIR}/cstring.h
	${CMAKE_CURRENT_SOURCE_DIR}/cstring_test.c
	${CMAKE_CURRENT_SOURCE_DIR}/atomic.h
	${CMAKE_CURRENT_SOURCE_DIR}/crt_dbg_leak.h
    ${CMAKE_CURRENT_SOURCE_DIR}/crt_dbg_leak.c
)
add_executable(test_string ${string_source_files})
if(WIN32)
	target_compile_options(test_string PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()
source_group(files FILES ${string_source_files})




set(nmutex_source_files 
	${CMAKE_CURRENT_SOURCE_DIR}/nmutex.c
	${CMAKE_CURRENT_SOURCE_DIR}/nmutex.h
	${CMAKE_CURRENT_SOURCE_DIR}/nmutex_test.c
	${CMAKE_CURRENT_SOURCE_DIR}/atomic.h
)
add_executable(test_nmutex ${nmutex_source_files})
if(WIN32)
	target_compile_options(test_nmutex PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
else()
	target_compile_definitions(test_nmutex PRIVATE _GNU_SOURCE)
	target_link_libraries(test_nmutex PRIVATE pthread)
endif()
source_group(files FILES ${nmutex_source_files})





set(fmem_Source_files 
	${CMAKE_CURRENT_SOURCE_DIR}/fmem.h
	${CMAKE_CURRENT_SOURCE_DIR}/fmem_tests.c
)
list(APPEND fmem_Source_files ${CMAKE_CURRENT_SOURCE_DIR}/fmem.c)
if(WIN32)
list(APPEND fmem_Source_files ${CMAKE_CURRENT_SOURCE_DIR}/fmem.cpp)
else()
endif()
add_executable(fmem_test ${fmem_Source_files})
if (WIN32)
  target_compile_options(fmem_test PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()
source_group(files FILES ${fmem_Source_files})



add_executable(test_sort 
	${CMAKE_CURRENT_SOURCE_DIR}/sort.h
	${CMAKE_CURRENT_SOURCE_DIR}/sort.c
	${CMAKE_CURRENT_SOURCE_DIR}/sort_test.c
)


add_executable(test_search 
	${CMAKE_CURRENT_SOURCE_DIR}/search.h
	${CMAKE_CURRENT_SOURCE_DIR}/search.c
)


# too slow
# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/curl_async/tests)


add_executable(test_asprintf
	${CMAKE_CURRENT_SOURCE_DIR}/asprintf.h
	${CMAKE_CURRENT_SOURCE_DIR}/asprintf.c
)
if(WIN32)
	target_compile_options(test_asprintf PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()



add_executable(test_ipaddress
	${CMAKE_CURRENT_SOURCE_DIR}/ipaddress.h
	${CMAKE_CURRENT_SOURCE_DIR}/ipaddress.c
)
if(WIN32)
	target_compile_options(test_ipaddress PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
endif()
