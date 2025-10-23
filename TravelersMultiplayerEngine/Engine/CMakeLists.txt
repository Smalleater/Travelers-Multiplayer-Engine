cmake_minimum_required(VERSION 3.10)

project(TME_Engine VERSION 0.1)

# Use C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Collect all source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Build as a shared library (.dll on Windows, .so on Linux)
add_library(tme_engine SHARED ${SOURCES})

# Set include directories (public headers and internal headers)
target_include_directories(tme_engine
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${COMMON_INCLUDE_DIR}
        ${COMMON_CORE_INCLUDE_DIR}
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/internal
)

# Define export macro for DLL/SO symbol visibility
target_compile_definitions(tme_engine PRIVATE TME_EXPORTS)

# Link with ws2_32 (Windows Sockets library) only on Windows
if(WIN32)
    target_link_libraries(tme_engine PRIVATE ws2_32)
endif()

target_compile_definitions(tme_engine
    PUBLIC
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
)

add_custom_command(TARGET tme_engine POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
    $<TARGET_FILE:tme_engine> ${COMMON_EXPORT_LIB_DIR}/)

add_custom_command(TARGET tme_engine POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
    $<TARGET_LINKER_FILE:tme_engine> ${COMMON_EXPORT_LIB_DIR}/)

file(GLOB HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/include/TME/engine/*")
add_custom_command(TARGET tme_engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${HEADER_FILES} ${COMMON_EXPORT_INCLUDE_DIR}/engine/)

file(GLOB HEADER_FILES "${COMMON_INCLUDE_DIR}/TME/*")
add_custom_command(TARGET tme_engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${HEADER_FILES} ${COMMON_EXPORT_INCLUDE_DIR}/)