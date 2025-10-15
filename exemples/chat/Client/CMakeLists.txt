cmake_minimum_required(VERSION 3.10)
project(Client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(client main_client.cpp)

set(TME_LIB_DIR "${CMAKE_SOURCE_DIR}/../lib")
set(TME_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/../include")

target_include_directories(client PRIVATE "${TME_INCLUDE_DIR}")

target_link_directories(client PRIVATE "${TME_LIB_DIR}")

if (WIN32)
    target_link_libraries(client PRIVATE "${TME_LIB_DIR}/libtme.dll.a")

    add_custom_command(TARGET client POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${TME_LIB_DIR}/libtme.dll"
        $<TARGET_FILE_DIR:client>)
else()
    target_link_libraries(client PRIVATE tme)

    add_custom_command(TARGET client POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${TME_LIB_DIR}/libtme.so"
        $<TARGET_FILE_DIR:client>)
endif()