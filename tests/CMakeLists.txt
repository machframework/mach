add_executable(mach_tests
    routing/RouteTrieTests.cpp
)

target_link_libraries(mach_tests
    PRIVATE
        mach
)

target_include_directories(mach_tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

add_test(NAME mach_tests COMMAND mach_tests)