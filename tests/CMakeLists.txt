cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

find_package(Qt5 CONFIG REQUIRED COMPONENTS Test)

foreach(tst IN ITEMS tst_qlegodevicescanner)
    add_executable(${tst} ${tst}.cpp ${tst}.h)
    target_link_libraries(${tst} PRIVATE Qt5::Lego Qt5::Test)
    add_test(NAME ${tst} COMMAND ${tst})
endforeach()
