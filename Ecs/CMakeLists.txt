cmake_minimum_required(VERSION 3.10)

project(TRA_Ecs VERSION 0.1)

# Use C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Collect all source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Build as a shared library (.dll on Windows, .so on Linux)
add_library(tra_ecs SHARED ${SOURCES})

# Set include directories (public headers and internal headers)
target_include_directories(tra_ecs
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/internal
)

# Define export macro for DLL/SO symbol visibility
target_compile_definitions(tra_ecs PRIVATE TRA_EXPORTS)

if(WIN32)
    target_link_libraries(tra_ecs PUBLIC ${COMMON_EXPORT_LIB_DIR}/tra_core.lib)
else()
    target_link_libraries(tra_ecs PUBLIC ${COMMON_EXPORT_LIB_DIR}/libtra_core.so)
endif()

target_link_libraries(tra_ecs PUBLIC tra_core)

target_compile_definitions(tra_ecs
    PUBLIC
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
)

add_custom_command(TARGET tra_ecs POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
    $<TARGET_FILE:tra_ecs> ${COMMON_EXPORT_LIB_DIR}/)

add_custom_command(TARGET tra_ecs POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
    $<TARGET_LINKER_FILE:tra_ecs> ${COMMON_EXPORT_LIB_DIR}/)

file(GLOB HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/include/TRA/ecs/*")
add_custom_command(TARGET tra_ecs POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${HEADER_FILES} ${COMMON_EXPORT_INCLUDE_DIR}/ecs/)

file(GLOB HEADER_FILES "${COMMON_INCLUDE_DIR}/TRA/*")
add_custom_command(TARGET tra_ecs POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${HEADER_FILES} ${COMMON_EXPORT_INCLUDE_DIR}/)