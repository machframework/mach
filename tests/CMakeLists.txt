function(add_mach_test bench_name)
    add_executable("${bench_name}"
        ${ARGN}
    )

    target_link_libraries("${bench_name}"
        PRIVATE mach
    )

    target_include_directories("${bench_name}"
        PRIVATE
            ${CMAKE_SOURCE_DIR}/src
            ${CMAKE_CURRENT_SOURCE_DIR}
    )

    add_test(
        NAME "${bench_name}"
        COMMAND "${bench_name}"
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

add_mach_test(
    BasicDITests
    di/BasicDITests.cpp
)

add_mach_test(
    DILifetimeTests
    di/DILifetimeTests.cpp
)

add_mach_test(
    ContainerTests
    di/ContainerTests.cpp
)

add_mach_test(
    CompileDITests.cpp
    di/CompileDITests.cpp
)
