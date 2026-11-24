# Test support library

add_library(mach_test_support STATIC
    support/BeastHttpClient.cpp
    support/TestRequestBuilder.cpp
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


# Test registration functions

function(add_mach_test_executable test_name)
    add_executable(
        ${test_name}
        ${ARGN}
    )

    target_link_libraries(
        ${test_name}
        PRIVATE
            mach
            mach_test_support
    )

    target_include_directories(
        ${test_name}
        PRIVATE
            ${CMAKE_SOURCE_DIR}/src
            ${CMAKE_CURRENT_SOURCE_DIR}
    )
endfunction()

function(add_mach_test test_name)
    add_mach_test_executable(${test_name} ${ARGN})

    add_test(
        NAME ${test_name}
        COMMAND ${test_name}
    )
endfunction()

function(add_mach_catch_test test_name)
    add_mach_test_executable(${test_name} ${ARGN})

    target_link_libraries(
        ${test_name}
        PRIVATE
            Catch2::Catch2WithMain
    )

    catch_discover_tests(${test_name})
endfunction()

function(add_mach_compile_failure_test test_name source diagnostic)
    add_test(
        NAME ${test_name}
        COMMAND "${CMAKE_COMMAND}"
            -S "${CMAKE_SOURCE_DIR}/cmake/compile_failure"
            -B "${CMAKE_CURRENT_BINARY_DIR}/compile_failure/${test_name}"
            "-DTEST_SOURCE=${CMAKE_CURRENT_SOURCE_DIR}/${source}"
            "-DMACH_INCLUDE_DIR=${CMAKE_SOURCE_DIR}/include"
            "-DEXPECTED_DIAGNOSTIC=${diagnostic}"
            "-DTEST_COMPILE_DEFINITIONS=${ARGN}"
            "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
            "-DCMAKE_CXX_COMPILER_TARGET=${CMAKE_CXX_COMPILER_TARGET}"
            "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
            "-DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}"
            "-DCMAKE_MSVC_RUNTIME_LIBRARY=${CMAKE_MSVC_RUNTIME_LIBRARY}"
            "-DDEPENDENCY_INCLUDE_DIRS=${dependency_include_dirs}"
            "-G${CMAKE_GENERATOR}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    )

    set_tests_properties(
        ${test_name}
        PROPERTIES
            LABELS "compile-failure"
    )
endfunction()


# Compile-failure test configuration

get_target_property(
    fmt_include_dirs
    fmt::fmt
    INTERFACE_INCLUDE_DIRECTORIES
)

get_target_property(
    json_include_dirs
    nlohmann_json::nlohmann_json
    INTERFACE_INCLUDE_DIRECTORIES
)

set(dependency_include_dirs
    ${fmt_include_dirs}
    ${json_include_dirs}
)

list(FILTER dependency_include_dirs EXCLUDE REGEX "^$")
list(REMOVE_DUPLICATES dependency_include_dirs)

if(MSVC)
    set(overflow_diagnostic "C2131")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(overflow_diagnostic "not a constant expression")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(overflow_diagnostic "constant expression")
endif()


# Routing

add_mach_catch_test(
    RoutingTests
    routing/RoutingTests.cpp
)


# Dependency injection

add_mach_catch_test(
    DITests
    di/DITests.cpp
)

add_mach_catch_test(
    DIThreadSafetyTests
    di/DIThreadSafetyTests.cpp
)

add_mach_test(
    DICompileTimeTests
    di/DICompileTimeTests.cpp
)

target_compile_definitions(
    DICompileTimeTests
    PRIVATE
        MACH_DI_COMPILE_FAILURE_TEST=0
)


# Controllers

add_mach_catch_test(
    ControllerTests
    controllers/ControllerTests.cpp
)


# Middleware

add_mach_catch_test(
    MiddlewareTests
    middleware/MiddlewareTests.cpp
)

add_mach_test(
    MiddlewareCompileTimeTests
    middleware/MiddlewareCompileTimeTests.cpp
)

target_compile_definitions(
    MiddlewareCompileTimeTests
    PRIVATE
        MACH_MIDDLEWARE_COMPILE_FAILURE_TEST=0
)


# Minimal APIs

add_mach_test(
    MinimalApiCompileTimeTests
    minimal-api/MinimalApiCompileTimeTests.cpp
)


# Validation

add_mach_catch_test(
    ValidationTests
    validation/ValidationTests.cpp
)


# Core

add_mach_catch_test(
    ByteLiteralsTests
    core/ByteLiteralsTests.cpp
)

add_mach_compile_failure_test(
    ByteLiteralsOverflowTests
    core/ByteLiteralsOverflowTests.cpp
    "${overflow_diagnostic}"
)


# Binding

add_mach_catch_test(
    BindingTests
    binding/BindingTests.cpp
)

add_mach_test(
    BindingTypesTests
    binding/BindingTypesTests.cpp
)

add_mach_test(
    BindingTypesCompileTimeTests
    binding/BindingTypesCompileTimeTests.cpp
)

add_mach_catch_test(
    RequestBinderTests
    binding/RequestBinderTests.cpp
)

add_mach_catch_test(
    MinimalApiBindingTests
    binding/MinimalApiBindingTests.cpp
)

add_mach_catch_test(
    ControllerBindingTests
    binding/ControllerBindingTests.cpp
)

add_mach_test(
    MinimalApiBindingCompileTimeTests
    binding/MinimalApiBindingCompileFailureTests.cpp
)

target_compile_definitions(
    MinimalApiBindingCompileTimeTests
    PRIVATE
        MACH_COMPILE_FAILURE_CASE=0
)

set(
    minimal_api_binding_diagnostic
    "Handler arguments must be Context&, const Context&, or unqualified binding types passed by value."
)

foreach(case RANGE 1 9)
    if(case EQUAL 7)
        set(
            expected_diagnostic
            "Handler must not accept more than one Context parameter."
        )
    else()
        set(expected_diagnostic "${minimal_api_binding_diagnostic}")
    endif()

    add_mach_compile_failure_test(
        MinimalApiBindingFailure${case}
        binding/MinimalApiBindingCompileFailureTests.cpp
        "${expected_diagnostic}"
        MACH_COMPILE_FAILURE_CASE=${case}
    )
endforeach()

add_mach_test(
    ControllerBindingCompileTimeTests
    binding/ControllerBindingCompileFailureTests.cpp
)

target_compile_definitions(
    ControllerBindingCompileTimeTests
    PRIVATE
        MACH_CONTROLLER_COMPILE_FAILURE_CASE=0
)

set(
    controller_binding_diagnostic
    "Action arguments must be unqualified binding types passed by value"
)

foreach(case RANGE 1 11)
    if(case EQUAL 4)
        set(
            expected_diagnostic
            "Action arguments must be unqualified binding types passed by value."
        )
    elseif(case EQUAL 7 OR case EQUAL 8 OR case EQUAL 10)
        set(
            expected_diagnostic
            "controller actions must not accept a Context parameter"
        )
    elseif(case EQUAL 9)
        set(
            expected_diagnostic
            "legacy controller actions must accept at most one body parameter"
        )
    else()
        set(expected_diagnostic "${controller_binding_diagnostic}")
    endif()

    add_mach_compile_failure_test(
        ControllerBindingFailure${case}
        binding/ControllerBindingCompileFailureTests.cpp
        "${expected_diagnostic}"
        MACH_CONTROLLER_COMPILE_FAILURE_CASE=${case}
    )
endforeach()


# HTTP

add_mach_catch_test(
    TestRequestBuilderTests
    http/TestRequestBuilderTests.cpp
)

add_mach_catch_test(
    RequestHeadersTests
    http/RequestHeadersTests.cpp
)

add_mach_catch_test(
    ResponseHeadersTests
    http/ResponseHeadersTests.cpp
)