cmake_minimum_required(VERSION 3.10)

project(TME_Client VERSION 0.1)

# Use C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Collect all source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Build as a shared library (.dll on Windows, .so on Linux)
add_library(tme_client SHARED ${SOURCES})

# Set include directories (public headers and internal headers)
target_include_directories(tme_client
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/internal
)

# Define export macro for DLL/SO symbol visibility
target_compile_definitions(tme_client PRIVATE TME_EXPORTS)

if(WIN32)
    target_link_libraries(tme_client PUBLIC ${COMMON_EXPORT_LIB_DIR}/tme_engine.lib)
else()
    target_link_libraries(tme_client PUBLIC ${COMMON_EXPORT_LIB_DIR}/libtme_engine.so)
endif()

target_link_libraries(tme_client PUBLIC tme_engine)

target_compile_definitions(tme_client
    PUBLIC
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
)

add_custom_command(TARGET tme_client POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
    $<TARGET_FILE:tme_client> ${COMMON_EXPORT_LIB_DIR}/)

add_custom_command(TARGET tme_client POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
    $<TARGET_LINKER_FILE:tme_client> ${COMMON_EXPORT_LIB_DIR}/)

file(GLOB HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/include/TME/client/*")
add_custom_command(TARGET tme_client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${HEADER_FILES} ${COMMON_EXPORT_INCLUDE_DIR}/client/)

file(GLOB HEADER_FILES "${COMMON_INCLUDE_DIR}/TME/*")
add_custom_command(TARGET tme_client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${HEADER_FILES} ${COMMON_EXPORT_INCLUDE_DIR}/)
