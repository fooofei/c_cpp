
cmake_minimum_required(VERSION 3.1)
project(C_MAKE_DEMO)




# more files, you can use if()
#list(APPEND Source_files )
set(dpdk_gen_home ${CMAKE_CURRENT_LIST_DIR}/../dpdk-stable-16.11.3/x86_64-native-linuxapp-gcc)
set(dpdk_lib_located ${dpdk_gen_home}/lib)
link_directories(${dpdk_lib_located})

set(self_1 master)
set(Source_files_1
	${CMAKE_CURRENT_LIST_DIR}/master.c
	${CMAKE_CURRENT_LIST_DIR}/header.h
	)
add_executable(${self_1} ${Source_files_1})

set(self_2 slave)
set(Source_files_2
	${CMAKE_CURRENT_LIST_DIR}/slave.c
    ${CMAKE_CURRENT_LIST_DIR}/header.h
	)
add_executable(${self_2} ${Source_files_2})

macro(dpdk_standard self)
    
    # include other directories
    target_include_directories(${self} PRIVATE ${dpdk_gen_home}/include)

    target_link_libraries(${self} "-Wl,--whole-archive -lrte_eal -lrte_mbuf -Wl,--no-whole-archive" )
    target_link_libraries(${self} "-Wl,--whole-archive -lrte_pmd_e1000 -Wl,--no-whole-archive" )
    target_link_libraries(${self} "-Wl,--whole-archive -lrte_mempool -lrte_ring -lrte_ethdev -Wl,--no-whole-archive" )
    target_link_libraries(${self} "-Wl,--whole-archive  -lrte_pmd_ixgbe -Wl,--no-whole-archive" )
    target_link_libraries(${self} "dl" )
    target_link_libraries(${self} "pthread" )

    target_compile_options(${self} PRIVATE "-msse4.2")

    ## defines options link-libraries
    #target_compile_definitions(${self_binary_name} PRIVATE "_USE_DATA2")
    if (WIN32)
    else()
      # fPIC for other library link
      target_compile_options(${self} PRIVATE -fPIC)
      target_compile_options(${self} PRIVATE -fvisibility=hidden)
      target_compile_options(${self} PRIVATE -fno-common )
    endif()

endmacro()

dpdk_standard(${self_1})
dpdk_standard(${self_2})