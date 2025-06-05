cmake_minimum_required(VERSION 3.15.0 FATAL_ERROR)

find_package(Qt5 CONFIG REQUIRED
    COMPONENTS Core Bluetooth
)

add_library(Lego
    qlegoglobal.h
    qlegodevice.h
    qlegodevice.cpp
    qlegodevicescanner.h
    qlegodevicescanner.cpp
    qlegoattacheddevice.h
    qlegoattacheddevice.cpp
    qlegomotor.h
    qlegomotor.cpp
)

add_library(Qt5::Lego ALIAS Lego)

set_target_properties(Lego PROPERTIES
    OUTPUT_NAME QtLego
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

target_include_directories(Lego
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/QtLego>
    PUBLIC
        $<INSTALL_INTERFACE:include>
)

target_compile_definitions(Lego PRIVATE QT_BUILD_LEGO_LIB)

target_link_libraries(Lego PUBLIC
    Qt5::Core
    Qt5::Bluetooth
)

# Install the library
install(TARGETS Lego
    EXPORT Lego
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# List of public headers to generate
set(PUBLIC_HEADERS
    QLegoGlobal
    QLegoDevice
    QLegoDeviceScanner
    QLegoAttachedDevice
    QLegoMotor
)

# Install headers
foreach(header ${PUBLIC_HEADERS})
    string(TOLOWER "${header}" header_name)
    set(input "${header_name}.h")
    set(output "include/QtLego/${header}")
    set(content "#include \"${header_name}.h\"")
    file(GENERATE OUTPUT "${output}" CONTENT ${content})
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${output}" "${input}"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/QtLego"
    )
endforeach()

# Install the CMake targets
install(EXPORT Lego
    DESTINATION ${CMAKE_INSTALL_CMAKEDIR}
    NAMESPACE Qt5::
    FILE Qt5LegoTargets.cmake
)
