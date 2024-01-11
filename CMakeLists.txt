cmake_minimum_required(VERSION 3.19)
project(RapidMenu)

set(CMAKE_CXX_STANDARD 23)

# Add the path to the source files
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")

# Add the executable target
add_executable(RapidMenu ${SOURCE_FILES})

# Find and link against the tomlplusplus library
find_package(tomlplusplus REQUIRED)
target_link_libraries(RapidMenu PRIVATE tomlplusplus::tomlplusplus)

