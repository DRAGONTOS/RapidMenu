cmake_minimum_required(VERSION 3.28.2)

# Get version
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/props.json PROPS)
string(JSON VER GET ${PROPS} version)

project(RapidMenu
  DESCRIPTION "Utilize configuration files to create runner scripts."
  VERSION ${VER})

# Find dependencies (cpptoml is header-only and ships no pkg-config file)
find_path(CPPTOML_INCLUDE_DIR cpptoml.h REQUIRED)

set(CMAKE_CXX_STANDARD 26)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release) 
endif()

# Add the path to the source files for RapidMenu
file(GLOB_RECURSE RAPIDMENU_SOURCE_FILES "src/*.cpp")

# Set compiler flags
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")  

# Add the executable target for RapidMenu
add_executable(RapidMenu ${RAPIDMENU_SOURCE_FILES})
install(TARGETS RapidMenu DESTINATION bin)
install(FILES assets/dashboard.conf DESTINATION share/doc/RapidMenu/examples)

# Include directories
target_include_directories(RapidMenu PRIVATE 
  ${CPPTOML_INCLUDE_DIR})

# Package configuration
set(CPACK_PACKAGE_VENDOR "DRAGONTOS")
set(CPACK_PACKAGE_DESCRIPTION ${PROJECT_DESCRIPTION})
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")

# DEBIAN specific settings
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "DRAGONTOS")
set(CPACK_DEBIAN_PACKAGE_LICENSE "GPLv3")
set(CPACK_DEBIAN_PACKAGE_RELEASE_DIST ON)

# RPM specific settings
set(CPACK_RPM_PACKAGE_LICENSE "GPLv3")
set(CPACK_RPM_PACKAGE_RELEASE_DIST ON)

# Set package generators (both DEB and RPM)
set(CPACK_GENERATOR "DEB;RPM")

include(CPack)
