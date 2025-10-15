cmake_minimum_required(VERSION 3.10)
project(Server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(server main_server.cpp)

set(TME_LIB_DIR "${CMAKE_SOURCE_DIR}/../lib")
set(TME_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/../include")

target_include_directories(server PRIVATE "${TME_INCLUDE_DIR}")

target_link_directories(server PRIVATE "${TME_LIB_DIR}")

if (WIN32)
    target_link_libraries(server PRIVATE "${TME_LIB_DIR}/libtme.dll.a")

    add_custom_command(TARGET server POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${TME_LIB_DIR}/libtme.dll"
        $<TARGET_FILE_DIR:server>)
else()
    target_link_libraries(server PRIVATE tme)

    add_custom_command(TARGET server POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${TME_LIB_DIR}/libtme.so"
        $<TARGET_FILE_DIR:server>)
endif()