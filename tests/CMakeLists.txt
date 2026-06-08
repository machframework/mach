add_executable(mach_tests
    routing/RouteTrieTests.cpp
)

target_link_libraries(mach_tests
    PRIVATE
        mach
)

add_test(NAME mach_tests COMMAND mach_tests)