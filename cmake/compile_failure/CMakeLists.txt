
cmake_minimum_required(VERSION 3.20)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(MachCompileFailure LANGUAGES CXX)

if(NOT DEFINED TEST_SOURCE OR NOT DEFINED EXPECTED_DIAGNOSTIC)
    message(FATAL_ERROR "Missing compile-failure test parameters")
endif()

try_compile(
    compiled
    "${CMAKE_CURRENT_BINARY_DIR}/check"
    "${TEST_SOURCE}"
    CMAKE_FLAGS
        "-DCMAKE_CXX_STANDARD=20"
        "-DCMAKE_CXX_STANDARD_REQUIRED=ON"
    COMPILE_DEFINITIONS
        "-I${MACH_INCLUDE_DIR}"
    OUTPUT_VARIABLE output
)

if(compiled)
    message(FATAL_ERROR "Expected compilation to fail, but it succeeded")
endif()

if(NOT output MATCHES "${EXPECTED_DIAGNOSTIC}")
    message(FATAL_ERROR
        "Compilation failed for an unexpected reason:\n${output}"
    )
endif()

message(STATUS "Compilation failed as expected")
