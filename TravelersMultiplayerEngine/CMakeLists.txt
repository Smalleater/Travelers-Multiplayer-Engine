cmake_minimum_required(VERSION 3.10)

project(TravelersMultiplayerEngine VERSION 0.1)

# Use C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Define common include path BEFORE subdirectories
set(COMMON_INCLUDE ${CMAKE_SOURCE_DIR}/CommonInclude)
set(COMMON_CORE_INCLUDE ${CMAKE_SOURCE_DIR}/Core/include)

add_subdirectory(Core)
add_subdirectory(Engine)