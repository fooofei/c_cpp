cmake_minimum_required(VERSION 3.0)
project(C_MAKE_DEMO C)

if(APPLE)
    # https://stackoverflow.com/questions/31561309/cmake-warnings-under-os-x-macosx-rpath-is-not-specified-for-the-following-targe
    set(CMAKE_MACOSX_RPATH 0)
endif()


# TODO
# list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_LIST_DIR}")
# include(libcurl_static)
#FIND_PACKAGE(libcurl_static) # the filename must be XXXConfig.cmake
# 放弃 curl CMake 使用方式，我们选择使用干净的静态库
set(curl_home ${CMAKE_CURRENT_LIST_DIR}/../../curl_bin)
link_directories(${curl_home}/lib)


# TODO
set(common_c_common ${CMAKE_CURRENT_LIST_DIR}/../..)
#set(common_c_common ${CMAKE_CURRENT_LIST_DIR}/../../)
set(common_source_files 
	${CMAKE_CURRENT_LIST_DIR}/curl_common.c
	${CMAKE_CURRENT_LIST_DIR}/curl_common.h
    ${common_c_common}/crt_dbg_leak.c
	${common_c_common}/crt_dbg_leak.h
	${common_c_common}/datetime.h
	${common_c_common}/datetime.c
)




set(self_simple_curl curl_simple)
add_executable(${self_simple_curl} 
	${CMAKE_CURRENT_LIST_DIR}/simple_curl.c
	${common_source_files}
)
target_include_directories(${self_simple_curl} PRIVATE ${common_c_common})
target_link_libraries(${self_simple_curl} curl) 
target_include_directories(${self_simple_curl} PRIVATE "${curl_home}/include")

if (WIN32)
  # visual studio file encoding
  target_compile_options(${self_simple_curl} PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
  target_compile_options(${self_simple_curl} PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/MP>)
#elseif(APPLE)
else()
  # fPIC for other library link
  target_link_libraries(${self_simple_curl} z) 
  target_link_libraries(${self_simple_curl} ldap) 
  target_link_libraries(${self_simple_curl} lber) 
  target_link_libraries(${self_simple_curl} pthread) 
  target_compile_options(${self_simple_curl} PRIVATE -fPIC)
  target_compile_options(${self_simple_curl} PRIVATE -fvisibility=hidden)
  target_compile_options(${self_simple_curl} PRIVATE -fno-common)
endif()








set(self_simple_async curl_async_test)
add_executable(${self_simple_async} 
	${CMAKE_CURRENT_LIST_DIR}/async_curl.c
	${CMAKE_CURRENT_LIST_DIR}/../curl_async.h
	${CMAKE_CURRENT_LIST_DIR}/../curl_async.c
	${common_source_files}
)
target_include_directories(${self_simple_async} PRIVATE ${common_c_common})
target_link_libraries(${self_simple_async} curl) 
# 无奈必须加这一句 目录包含的测试请看 libcurl_static.cmake
target_include_directories(${self_simple_async} PRIVATE "${curl_home}/include")

if (WIN32)
  # visual studio file encoding
  target_compile_options(${self_simple_async} PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
  target_compile_options(${self_simple_async} PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/MP>)
#elseif(APPLE)
else()
  # fPIC for other library link
  target_link_libraries(${self_simple_async} z) 
  target_link_libraries(${self_simple_async} ldap) 
  target_link_libraries(${self_simple_async} lber) 
  target_link_libraries(${self_simple_async} pthread) 
  target_compile_options(${self_simple_async} PRIVATE -fPIC)
  target_compile_options(${self_simple_async} PRIVATE -fvisibility=hidden)
  target_compile_options(${self_simple_async} PRIVATE -fno-common)
endif()




set(self_noblock_curl curl_noblock)
add_executable(${self_noblock_curl} 
	${CMAKE_CURRENT_LIST_DIR}/noblock_curl.c
	${common_source_files}
)
target_include_directories(${self_noblock_curl} PRIVATE ${common_c_common})
target_link_libraries(${self_noblock_curl} curl) 
target_include_directories(${self_noblock_curl} PRIVATE "${curl_home}/include")

if (WIN32)
  # visual studio file encoding
  target_compile_options(${self_noblock_curl} PRIVATE /source-charset:utf-8 /execution-charset:utf-8)
  target_compile_options(${self_noblock_curl} PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/MP>)
#elseif(APPLE)
else()
  # fPIC for other library link
  target_link_libraries(${self_noblock_curl} z) 
  target_link_libraries(${self_noblock_curl} ldap) 
  target_link_libraries(${self_noblock_curl} lber) 
  target_link_libraries(${self_noblock_curl} pthread) 
  target_compile_options(${self_noblock_curl} PRIVATE -fPIC)
  target_compile_options(${self_noblock_curl} PRIVATE -fvisibility=hidden)
  target_compile_options(${self_noblock_curl} PRIVATE -fno-common)
endif()

