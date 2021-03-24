cmake_minimum_required(VERSION 3.15.0 FATAL_ERROR)

set(CMAKE_BUILD_TYPE_INIT "Debug")

project(QtLego
    VERSION "0.1.0"
    DESCRIPTION "Qt Library for LEGO devices"
    HOMEPAGE_URL "https://github.com/Symbitic/QtLego"
    LANGUAGES CXX C
)

if(CMAKE_BUILD_TYPE MATCHES "[Dd]ebug")
    option(ENABLE_TESTS "Enable unit tests" ON)
    option(ENABLE_EXAMPLES "Enable examples" OFF)
else()
    option(ENABLE_TESTS "Enable unit tests" OFF)
    option(ENABLE_EXAMPLES "Enable examples" ON)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)

include(CMakePackageConfigHelpers)
include(FeatureSummary)
include(GNUInstallDirs)
include(InstallRequiredSystemLibraries)

# Determine install location for CMake files
if(WIN32)
    set(CMAKE_INSTALL_CMAKEDIR CMake
        CACHE STRING "Installation directory for CMake files")
else()
    set(CMAKE_INSTALL_CMAKEDIR ${CMAKE_INSTALL_LIBDIR}/cmake/Qt5Lego
        CACHE STRING "Installation directory for CMake files")
endif()

add_subdirectory(src)

if(ENABLE_EXAMPLES)
    add_subdirectory(examples)
endif()

if(ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Generate local targets file
export(TARGETS Lego
    NAMESPACE Qt5::
    FILE "${CMAKE_CURRENT_BINARY_DIR}/QtLegoDepends.cmake")

# Generate CMake packages
configure_package_config_file(QtLegoConfig.cmake.in QtLegoConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_CMAKEDIR})
write_basic_package_version_file(QtLegoConfigVersion.cmake
    COMPATIBILITY AnyNewerVersion)

# Install the CMake config files
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/QtLegoConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/QtLegoConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_CMAKEDIR}
)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

set(PROJECT_CONTACT "Alex Shaw <alex.shaw.as@gmail.com>")
set(PROJECT_VENDOR "Alex Shaw")
set(PROJECT_README ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
set(PROJECT_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.md)
set(PROJECT_LICENSE_NAME "MIT")
set(PROJECT_HOMEPAGE "https://github.com/Symbitic/QtLego")

include(CPack)
