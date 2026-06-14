function(add_mach_test test_name)
    add_executable("${test_name}"
        ${ARGN}
    )

    target_link_libraries("${test_name}"
        PRIVATE mach
    )

    target_include_directories("${test_name}"
        PRIVATE
            ${CMAKE_SOURCE_DIR}/src
            ${CMAKE_CURRENT_SOURCE_DIR}
    )

    add_test(
        NAME "${test_name}"
        COMMAND "${test_name}"
    )
endfunction()

add_mach_test(
    RouteTrieTests
    routing/RouteTrieTests.cpp
)

add_mach_test(
    RouteParametersTests
    routing/RouteParametersTests.cpp
)

add_mach_test(
    ConstrainedRoutesTests
    routing/ConstrainedRoutesTests.cpp
)

add_mach_test(
    RouteSyntaxTests
    routing/RouteSyntaxTests.cpp
)

add_mach_test(
    RoutingTests
    routing/RoutingTests.cpp
)

add_mach_test(
    RoutingRuntimeTests
    routing/RoutingRuntimeTests.cpp
)