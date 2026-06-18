cmake_minimum_required(VERSION 3.25)

project(Mach VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Boost REQUIRED)

# -------------------------
# Core library
# -------------------------
add_library(mach
    src/server/BeastListener.cpp
    src/server/BeastSession.cpp
    src/logging/Logging.cpp
    src/server/Server.cpp
    src/http/Request.cpp
    src/http/Response.cpp
    src/adapter/inbound/BeastRequestAdapter.cpp
    src/adapter/outbound/BeastResponseAdapter.cpp
    src/application/Runtime.cpp
    src/routing/Router.cpp
    src/routing/RouteTrie.cpp
    src/app/App.cpp
    src/dispatching/Dispatcher.cpp 
    src/routing/RouteConstraint.cpp
    src/di/Scope.cpp
    src/di/Container.cpp
)

target_include_directories(mach
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(mach
    PUBLIC
        Boost::headers
)

option(MACH_BUILD_EXAMPLES "Build Mach examples" ON)
option(MACH_BUILD_TESTS "Build Mach tests" OFF)
option(MACH_BUILD_BENCHMARKS "Build Mach benchmarks" OFF)

if(MACH_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(MACH_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(MACH_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()