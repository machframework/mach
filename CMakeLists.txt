cmake_minimum_required(VERSION 3.25)

project(
    Mach
    VERSION 0.7.0
    DESCRIPTION "A modern C++ web framework"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

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

add_library(Mach::Mach ALIAS mach)

set_target_properties(mach PROPERTIES
    EXPORT_NAME Mach
)

target_include_directories(mach
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
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

# -------------------------
# Installation
# -------------------------
set(MACH_CMAKE_INSTALL_DIR
    "${CMAKE_INSTALL_LIBDIR}/cmake/Mach"
)

install(
    TARGETS mach
    EXPORT MachTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/mach
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    EXPORT MachTargets
    FILE MachTargets.cmake
    NAMESPACE Mach::
    DESTINATION ${MACH_CMAKE_INSTALL_DIR}
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/MachConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/MachConfig.cmake"
    INSTALL_DESTINATION ${MACH_CMAKE_INSTALL_DIR}
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/MachConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/MachConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/MachConfigVersion.cmake"
    DESTINATION ${MACH_CMAKE_INSTALL_DIR}
)

# -------------------------
# Optional components
# -------------------------
option(MACH_BUILD_EXAMPLES "Build Mach examples" OFF)
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