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
    CompileDITests
    di/CompileDITests.cpp
)

add_mach_test(
    BasicControllerTests
    controllers/BasicControllerTests.cpp
)

add_mach_test(
    BasicMiddlewareTests
    middleware/BasicMiddlewareTests.cpp
)

add_mach_test(
    BeastHardeningTests
    hardening/server/main.cpp
)

add_mach_test(
    InboundAdapterTests
    hardening/adapters/InboundAdapterTests.cpp
)

add_mach_test(
    RouteRegistrationAmbiguityTests
    routing/RouteRegistrationAmbiguityTests.cpp
)

add_mach_test(
    CompileTimeGuards
    hardening/di/CompileTimeGuards.cpp
)

set(MACH_DI_COMPILE_FAILURE_TEST 16)

target_compile_definitions(
    CompileTimeGuards
    PRIVATE
        MACH_DI_COMPILE_FAILURE_TEST=${MACH_DI_COMPILE_FAILURE_TEST}
)