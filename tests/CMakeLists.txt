add_library(mach_test_support STATIC
    support/BeastHttpClient.cpp
)

target_include_directories(
    mach_test_support
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/support

    PRIVATE
        ${MACH_BOOST_INCLUDE_DIR}
)

target_link_libraries(
    mach_test_support
    PRIVATE
        mach
)

include(Catch)

function(add_mach_test test_name)
    add_executable(
        "${test_name}"
        ${ARGN}
    )

    target_link_libraries(
        "${test_name}"
        PRIVATE
            mach
            mach_test_support
    )

    target_include_directories(
        "${test_name}"
        PRIVATE
            ${CMAKE_SOURCE_DIR}/src
            ${CMAKE_CURRENT_SOURCE_DIR}
    )

    add_test(
        NAME "${test_name}"
        COMMAND "${test_name}"
    )
endfunction()

function(add_mach_compile_failure_test test_name source diagnostic)
    add_test(
        NAME "${test_name}"
        COMMAND "${CMAKE_COMMAND}"
            -S "${CMAKE_SOURCE_DIR}/cmake/compile_failure"
            -B "${CMAKE_CURRENT_BINARY_DIR}/compile_failure/${test_name}"
            "-DTEST_SOURCE=${CMAKE_CURRENT_SOURCE_DIR}/${source}"
            "-DMACH_INCLUDE_DIR=${CMAKE_SOURCE_DIR}/include"
            "-DEXPECTED_DIAGNOSTIC=${diagnostic}"
            "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
            "-DCMAKE_CXX_COMPILER_TARGET=${CMAKE_CXX_COMPILER_TARGET}"
            "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
            "-DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}"
            "-DCMAKE_MSVC_RUNTIME_LIBRARY=${CMAKE_MSVC_RUNTIME_LIBRARY}"
            "-G${CMAKE_GENERATOR}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    )

    set_tests_properties("${test_name}" PROPERTIES
        LABELS "compile-failure"
    )
endfunction()

if(MSVC)
    set(overflow_diagnostic "C2131")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(overflow_diagnostic "not a constant expression")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(overflow_diagnostic "constant expression")
endif()

add_mach_test(
    RoutingTests
    routing/RoutingTests.cpp
)

target_link_libraries(
    RoutingTests
    PRIVATE
        mach
        mach_test_support
        Catch2::Catch2WithMain
)

catch_discover_tests(RoutingTests)

add_mach_test(
    DITests
    di/DITests.cpp
)

target_link_libraries(
    DITests
    PRIVATE
        mach
        mach_test_support
        Catch2::Catch2WithMain
)

catch_discover_tests(DITests)

add_mach_test(
    DIThreadSafetyTests
    di/DIThreadSafetyTests.cpp
)

target_link_libraries(
    DIThreadSafetyTests
    PRIVATE
        mach
        mach_test_support
        Catch2::Catch2WithMain
)

catch_discover_tests(DIThreadSafetyTests)

add_mach_test(
    DICompileTimeTests
    di/DICompileTimeTests.cpp
)

set(MACH_DI_COMPILE_FAILURE_TEST 0)

target_compile_definitions(
    DICompileTimeTests
    PRIVATE
        MACH_DI_COMPILE_FAILURE_TEST=${MACH_DI_COMPILE_FAILURE_TEST}
)

add_mach_test(
    ControllerTests
    controllers/ControllerTests.cpp
)

target_link_libraries(
    ControllerTests
    PRIVATE
        mach
        mach_test_support
        Catch2::Catch2WithMain
)

catch_discover_tests(ControllerTests)

add_mach_test(
    MiddlewareTests
    middleware/MiddlewareTests.cpp
)

target_link_libraries(
    MiddlewareTests
    PRIVATE
        mach
        mach_test_support
        Catch2::Catch2WithMain
)

catch_discover_tests(MiddlewareTests)

add_mach_test(
    MiddlewareCompileTimeTests
    middleware/MiddlewareCompileTimeTests.cpp
)

set(MACH_MIDDLEWARE_COMPILE_FAILURE_TEST 0)

target_compile_definitions(
    MiddlewareCompileTimeTests
    PRIVATE
        MACH_MIDDLEWARE_COMPILE_FAILURE_TEST=${MACH_MIDDLEWARE_COMPILE_FAILURE_TEST}
)

add_mach_test(
    MinimalApiCompileTimeTests
    minimal-api/MinimalApiCompileTimeTests.cpp
)

add_mach_test(
    BindingTests
    binding/BindingTests.cpp
)

target_link_libraries(
    BindingTests
    PRIVATE
        mach
        mach_test_support
        Catch2::Catch2WithMain
)

catch_discover_tests(BindingTests)

add_mach_test(
    ValidationTests
    validation/ValidationTests.cpp
)

target_link_libraries(
    ValidationTests
    PRIVATE
        mach
        mach_test_support
        Catch2::Catch2WithMain
)

catch_discover_tests(ValidationTests)

add_mach_test(
    ByteLiteralsTests
    core/ByteLiteralsTests.cpp
)

target_link_libraries(
    ByteLiteralsTests
    PRIVATE
        mach
        mach_test_support
        Catch2::Catch2WithMain
)

catch_discover_tests(ByteLiteralsTests)

add_mach_compile_failure_test(
    ByteLiteralsOverflowTests
    core/ByteLiteralsOverflowTests.cpp
    "${overflow_diagnostic}"
)
