cmake_minimum_required(VERSION 3.19)

# Get version
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/props.json PROPS)
string(JSON VER GET ${PROPS} version)

project(RapidMenu
  DESCRIPTION "Utilize configuration files to create runner scripts."
  VERSION ${VER})

set(CMAKE_CXX_STANDARD 23)

# Add the path to the source files for RapidMenu
file(GLOB_RECURSE RAPID_MENU_SOURCE_FILES "src/*.cpp")

# Add the executable target for RapidMenu
add_executable(RapidMenu ${RAPID_MENU_SOURCE_FILES})
install(TARGETS RapidMenu DESTINATION bin)

# Find and link against the tomlplusplus library
find_package(tomlplusplus REQUIRED)
target_link_libraries(RapidMenu PRIVATE tomlplusplus::tomlplusplus)


# DEBIAN
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "DRAGONTOS")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libcpptoml-dev")
set(CPACK_DEBIAN_PACKAGE_LICENSE "GPLv3")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION ${DESCRIPTION})
set(CPACK_DEBIAN_PACKAGE_RELEASE_DIST ON)
set(CPACK_GENERATOR DEBIAN)
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")
# RPM
set(CPACK_PACKAGE_VENDOR "DRAGONTOS")
set(CPACK_RPM_PACKAGE_REQUIRES "tomlplusplus-devel")
set(CPACK_RPM_PACKAGE_LICENSE "GPLv3")
set(CPACK_RPM_PACKAGE_DESCRIPTION ${DESCRIPTION})
set(CPACK_RPM_FILE_NAME "RPM-DEFAULT")
set(CPACK_RPM_PACKAGE_RELEASE_DIST ON)
set(CPACK_GENERATOR RPM)
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")
include(CPack)
