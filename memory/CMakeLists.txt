
cmake_minimum_required(VERSION 3.1)
project(memory)

set(TARGET1 mdll)
set(TARGET2 mexe)

 add_library(${TARGET1} SHARED dll_main.c)

 if(WIN32)
   target_sources(${TARGET1} PRIVATE Source.def)
 endif()


 add_executable(${TARGET2} exe_main.c)