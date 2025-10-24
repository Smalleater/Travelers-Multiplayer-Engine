cmake_minimum_required(VERSION 3.10)

project(TravelersMultiplayerEngine VERSION 0.1)

# Use C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Define common include path BEFORE subdirectories
set(COMMON_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/CommonInclude)
set(COMMON_CORE_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/Core/include)
set(COMMON_ENGINE_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/Engine/include)

set(COMMON_EXPORT_LIB_DIR ${CMAKE_SOURCE_DIR}/ExportDLL/lib)
set(COMMON_EXPORT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/ExportDLL/include/TME)

file(MAKE_DIRECTORY ${COMMON_EXPORT_LIB_DIR})
file(MAKE_DIRECTORY ${COMMON_EXPORT_INCLUDE_DIR})
file(MAKE_DIRECTORY ${COMMON_EXPORT_INCLUDE_DIR}/core)
file(MAKE_DIRECTORY ${COMMON_EXPORT_INCLUDE_DIR}/engine)
file(MAKE_DIRECTORY ${COMMON_EXPORT_INCLUDE_DIR}/server)

add_subdirectory(Core)
add_subdirectory(Engine)
add_subdirectory(Server)