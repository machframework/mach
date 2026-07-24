cmake_minimum_required(VERSION 3.25)

project(Mach VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Boost REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

# -------------------------
# Core library
# -------------------------
add_library(mach
    src/server/BeastListener.cpp
    src/server/BeastSession.cpp
    
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
    src/app/AppBuilder.cpp
    src/middleware/MiddlewarePipeline.cpp
    src/routing/RoutingMiddleware.cpp
    src/diagnostics/TerminateHandler.cpp
    src/controllers/ControllerBase.cpp
    src/middleware/Next.cpp
    src/exceptions/ExceptionMiddleware.cpp
    src/http/ContentType.cpp
    src/logging/Logger.cpp
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
        nlohmann_json::nlohmann_json
        fmt::fmt
    PRIVATE
        spdlog::spdlog
)

option(MACH_BUILD_EXAMPLES "Build Mach examples" ON)
option(MACH_BUILD_TESTS "Build Mach tests" ON)
option(MACH_BUILD_BENCHMARKS "Build Mach benchmarks" ON)

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