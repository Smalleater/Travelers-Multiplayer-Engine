cmake_minimum_required(VERSION 3.10)

project(TravelersMultiplayerEngine VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Collect all source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Build as a shared library (.dll on Windows, .so on Linux)
add_library(tme SHARED ${SOURCES})

# Set include directories (public headers and internal headers)
target_include_directories(tme
    PUBLIC ${PROJECT_SOURCE_DIR}/include
    PRIVATE ${PROJECT_SOURCE_DIR}/internal
)

# Define export macro for DLL/SO symbol visibility
target_compile_definitions(tme PRIVATE TME_EXPORTS)

#Optional: install public headers for users
install(DIRECTORY include/ DESTINATION include)