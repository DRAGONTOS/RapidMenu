cmake_minimum_required(VERSION 3.19)
project(RapidMenu)

set(CMAKE_CXX_STANDARD 23)

# Add the path to the source files for RapidMenu
file(GLOB_RECURSE RAPID_MENU_SOURCE_FILES "src/*.cpp")

# Add the executable target for RapidMenu
add_executable(RapidMenu ${RAPID_MENU_SOURCE_FILES})

# Find and link against the tomlplusplus library
find_package(tomlplusplus REQUIRED)
target_link_libraries(RapidMenu PRIVATE tomlplusplus::tomlplusplus)

