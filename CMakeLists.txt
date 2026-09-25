cmake_minimum_required(VERSION 3.26)

project(
    Mach
    VERSION 1.0.0
    DESCRIPTION "A modern C++ web framework"
    LANGUAGES CXX
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# -------------------------
# Dependencies
# -------------------------
include(cmake/Dependencies.cmake)

# -------------------------
# Core library
# -------------------------
add_library(mach
    src/server/BeastListener.cpp
    src/server/BeastSession.cpp
    src/server/Server.cpp

    src/http/Request.cpp
    src/http/Response.cpp
    src/http/ContentType.cpp
    src/http/Context.cpp

    src/adapter/inbound/BeastRequestAdapter.cpp
    src/adapter/outbound/BeastResponseAdapter.cpp

    src/application/Runtime.cpp

    src/routing/Router.cpp
    src/routing/RouteTrie.cpp
    src/routing/RouteConstraint.cpp
    src/routing/RoutingMiddleware.cpp

    src/app/App.cpp
    src/app/AppBuilder.cpp

    src/dispatching/Dispatcher.cpp

    src/di/Scope.cpp
    src/di/Container.cpp

    src/middleware/MiddlewarePipeline.cpp
    src/middleware/Next.cpp

    src/controllers/ControllerBase.cpp

    src/exceptions/ExceptionMiddleware.cpp

    src/diagnostics/TerminateHandler.cpp

    src/logging/Logger.cpp
 
    src/exceptions/HttpException.cpp
    src/exceptions/NotFoundException.cpp
    src/exceptions/UnauthorizedException.cpp
    src/exceptions/ForbiddenException.cpp
    src/exceptions/ConflictException.cpp
    src/exceptions/BadRequestException.cpp
 
    src/validation/ValidationResult.cpp
    src/validation/ValidationUtils.cpp
    src/validation/Validator.cpp
 
    src/utility/StringUtils.cpp
    src/utility/HttpDate.cpp

    src/cors/CorsBuilder.cpp
    src/cors/CorsMiddleware.cpp

    src/csrf/CsrfMiddleware.cpp
    src/csrf/CsrfBuilder.cpp

    src/results/Results.cpp
)

set_target_properties(mach PROPERTIES
    DEBUG_POSTFIX "d"
)

add_library(Mach::Mach ALIAS mach)

set_target_properties(mach PROPERTIES
    EXPORT_NAME Mach
    CXX_EXTENSIONS NO
)

target_compile_features(mach
    PUBLIC
        cxx_std_20
)

target_include_directories(mach
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${MACH_BOOST_INCLUDE_DIR}
)

target_link_libraries(mach
    PUBLIC
        nlohmann_json::nlohmann_json
        fmt::fmt
    PRIVATE
        spdlog::spdlog
)

if(WIN32)
    target_link_libraries(mach
        PRIVATE
            bcrypt
            ws2_32
            mswsock
    )
endif()

# Boost.Asio requires the target Windows version to be declared.
# This definition has no effect on non-Windows platforms.
if(WIN32)
    target_compile_definitions(mach
        PUBLIC
            _WIN32_WINNT=0x0A00
    )
endif()

# -------------------------
# Installation
# -------------------------
if(MACH_CAN_INSTALL)

    set(
        MACH_CMAKE_INSTALL_DIR
        "${CMAKE_INSTALL_LIBDIR}/cmake/Mach"
    )

    set(MACH_VCPKG_TRIPLET "${VCPKG_TARGET_TRIPLET}")

    install(
        TARGETS mach
        EXPORT MachTargets

        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"

        INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    )

    install(
        DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/mach"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    )

    install(
        EXPORT MachTargets
        FILE MachTargets.cmake
        NAMESPACE Mach::
        DESTINATION "${MACH_CMAKE_INSTALL_DIR}"
    )

    if(WIN32)
    set(
        MACH_DEPENDENCY_INSTALL_DIR
        "dependencies/${VCPKG_TARGET_TRIPLET}"
    )
    elseif(UNIX AND NOT APPLE)
        set(
            MACH_DEPENDENCY_INSTALL_DIR
            "${CMAKE_INSTALL_LIBDIR}/mach/dependencies/${VCPKG_TARGET_TRIPLET}"
        )
    endif()

    configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/MachConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/MachConfig.cmake"
        INSTALL_DESTINATION "${MACH_CMAKE_INSTALL_DIR}"
    )

    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/MachConfigVersion.cmake"
        VERSION "${PROJECT_VERSION}"
        COMPATIBILITY SameMajorVersion
    )

    install(
        FILES
            "${CMAKE_CURRENT_BINARY_DIR}/MachConfig.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/MachConfigVersion.cmake"
        DESTINATION "${MACH_CMAKE_INSTALL_DIR}"
    )

endif()

option(
    MACH_BUILD_PACKAGE
    "Enable Mach standalone package generation"
    OFF
)

if(MACH_CAN_INSTALL AND MACH_BUILD_PACKAGE)

    set(
        MACH_CLI_EXECUTABLE
        ""
        CACHE FILEPATH
        "Path to the prebuilt Mach CLI executable"
    )

    include(cmake/Packaging.cmake)

endif()

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
    message(STATUS "Building Mach benchmarks")
    add_subdirectory(benchmarks)
endif()