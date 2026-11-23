
cmake_minimum_required(VERSION 3.20)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(MachCompileFailure LANGUAGES CXX)

if(NOT DEFINED TEST_SOURCE OR
   NOT DEFINED EXPECTED_DIAGNOSTIC OR
   EXPECTED_DIAGNOSTIC STREQUAL "")
    message(FATAL_ERROR "Missing compile-failure test parameters")
endif()

set(compile_definitions "-I${MACH_INCLUDE_DIR}")

# Add external dependency include directories.
foreach(include_dir IN LISTS DEPENDENCY_INCLUDE_DIRS)
    if(NOT include_dir STREQUAL "")
        list(APPEND compile_definitions "-I${include_dir}")
    endif()
endforeach()

# Add test-specific preprocessor definitions.
foreach(definition IN LISTS TEST_COMPILE_DEFINITIONS)
    list(APPEND compile_definitions "-D${definition}")
endforeach()

# fmt requires UTF-8 support when compiling with MSVC.
if(MSVC)
    list(APPEND compile_definitions "/utf-8")
endif()

try_compile(
    compiled
    "${CMAKE_CURRENT_BINARY_DIR}/check"
    "${TEST_SOURCE}"
    CMAKE_FLAGS
        "-DCMAKE_CXX_STANDARD=20"
        "-DCMAKE_CXX_STANDARD_REQUIRED=ON"
    COMPILE_DEFINITIONS
        ${compile_definitions}
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
