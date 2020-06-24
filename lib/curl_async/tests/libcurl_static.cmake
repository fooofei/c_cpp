
set(libcurl_name libcurl)


# overwrite the options
set(USE_MANUAL  OFF CACHE "" BOOL FORCE)
set(BUILD_TESTING  OFF CACHE "" BOOL FORCE)
set(CURL_WERROR ON CACHE "" BOOL FORCE)
set(BUILD_CURL_EXE  OFF CACHE "" BOOL FORCE)
set(CURL_STATICLIB ON CACHE "" BOOL FORCE)
if(WIN32)
  #set(CURL_STATIC_CRT ON CACHE "" BOOL FORCE)
  # msvc will have warning >LINK : warning LNK4098: 默认库“LIBCMTD”与其他库的使用冲突；请使用 /NODEFAULTLIB:library
  # 这里也用了 libcurl http://blog.51cto.com/aaroncao/1586552 
endif()

# 为什么在 curl/CMakeLists.txt 写了 option(CURL_STATICLIB  ON)  这个变量还是 OFF 呢
# 因为正确写法是  option(CURL_STATICLIB  "" ON)





if(NOT TARGET ${libcurl_name})
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/curl build_libcurl)
target_compile_definitions(${libcurl_name} PRIVATE "BUILDING_LIBCURL")
if (WIN32)
  ## from https://curl.haxx.se/docs/install.html
  target_compile_definitions(${libcurl_name} PUBLIC "CURL_STATICLIB")
elseif(APPLE)
  # macOS $ cmake -DOPENSSL_ROOT_DIR=/usr/local/opt/openssl ..
  #set(OPENSSL_ROOT_DIR /usr/local/opt/openssl)
endif()
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/curl  build_libcurl EXCLUDE_FROM_ALL)
endif()

# 奇怪 curl/lib/CMakeLists.txt 中有 target_include_directories
# 但是缺少下句 还是找不到 curl.h
# 多了下句又会报错 
#target_include_directories(${libcurl_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/curl/include)

## to see the target libcurl's include directory 
# when other module include it
#get_target_property(dirs libcurl INCLUDE_DIRECTORIES)
#foreach(dir ${dirs})
#  message(STATUS "[+] INCLUDE_DIRECTORIES1='${dir}'")
#endforeach()

## this print no value
#GET_DIRECTORY_PROPERTY(dirs INCLUDE_DIRECTORIES)
#message(STATUS "[+] INCLUDE_DIRECTORIES2='${dirs}'")

#
# target_link_libraries(${self_binary_name} ${libcurl_name})  


