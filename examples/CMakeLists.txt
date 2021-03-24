cmake_minimum_required(VERSION 3.15.0 FATAL_ERROR)

find_package(Qt5 CONFIG REQUIRED Core Bluetooth)

add_executable(helloworld helloworld.cpp)

target_link_libraries(helloworld PRIVATE
    Qt5::Core
    Qt5::Bluetooth
    Lego
)
