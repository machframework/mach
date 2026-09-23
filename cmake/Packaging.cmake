# Shared package metadata

set(CPACK_PACKAGE_NAME "Mach")
set(CPACK_PACKAGE_VENDOR "Mach")
set(
    CPACK_PACKAGE_DESCRIPTION_SUMMARY
    "A modern C++ web framework"
)
set(
    CPACK_PACKAGE_HOMEPAGE_URL
    "https://machframework.dev"
)

set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")


# Windows

if(WIN32)
    set(CPACK_GENERATOR "WIX")
    set(CPACK_WIX_ROOT "C:/Program Files (x86)/WiX Toolset v3.14")

    set(CPACK_PACKAGE_INSTALL_DIRECTORY "Mach")

    set(
        CPACK_RESOURCE_FILE_LICENSE
        "${CMAKE_SOURCE_DIR}/packaging/windows/LICENSE.rtf"
    )

    set(
        CPACK_WIX_UPGRADE_GUID
        "97A0CA68-0215-4513-A8B3-F4DBB6BB6977"
    )

    set(
        CPACK_WIX_PRODUCT_ICON
        "${CMAKE_SOURCE_DIR}/packaging/windows/assets/mach.ico"
    )

    set(
        CPACK_WIX_UI_BANNER
        "${CMAKE_SOURCE_DIR}/packaging/windows/assets/banner.bmp"
    )

    set(
        CPACK_WIX_UI_DIALOG
        "${CMAKE_SOURCE_DIR}/packaging/windows/assets/dialog.bmp"
    )

    set(
        CPACK_WIX_PATCH_FILE
        "${CMAKE_SOURCE_DIR}/packaging/windows/PathPatch.xml"
    )

    if(NOT MACH_CLI_EXECUTABLE)
        message(FATAL_ERROR
            "MACH_CLI_EXECUTABLE must be provided when creating the Windows package."
        )
    endif()

    if(NOT EXISTS "${MACH_CLI_EXECUTABLE}")
        message(FATAL_ERROR
            "Mach CLI executable not found: ${MACH_CLI_EXECUTABLE}"
        )
    endif()

    if(NOT MACH_CLI_TEMPLATE_DIR)
        message(FATAL_ERROR
            "MACH_CLI_TEMPLATE_DIR must be provided when creating the Windows package."
        )
    endif()

    if(NOT EXISTS "${MACH_CLI_TEMPLATE_DIR}")
        message(FATAL_ERROR
            "Mach CLI template directory not found: ${MACH_CLI_TEMPLATE_DIR}"
        )
    endif()

    install(
        PROGRAMS "${MACH_CLI_EXECUTABLE}"
        DESTINATION "${CMAKE_INSTALL_BINDIR}"
        RENAME "mach.exe"
    )

    install(
        DIRECTORY "${MACH_CLI_TEMPLATE_DIR}/"
        DESTINATION "templates"
    )

    install(
        DIRECTORY
            "${CMAKE_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/debug/"
        DESTINATION
            "dependencies/${VCPKG_TARGET_TRIPLET}/debug"
    )

    install(
        DIRECTORY
            "${CMAKE_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/include/"
        DESTINATION
            "dependencies/${VCPKG_TARGET_TRIPLET}/include"
    )

    install(
        DIRECTORY
            "${CMAKE_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/lib/"
        DESTINATION
            "dependencies/${VCPKG_TARGET_TRIPLET}/lib"
    )

    install(
        DIRECTORY
            "${CMAKE_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/share/"
        DESTINATION
            "dependencies/${VCPKG_TARGET_TRIPLET}/share"
    )

    if(WIN32)
        set(MACH_DEBUG_LIBRARY_PATH "lib/machd.lib")
    elseif(UNIX AND NOT APPLE)
        set(
            MACH_DEBUG_LIBRARY_PATH
            "${CMAKE_INSTALL_LIBDIR}/libmachd.a"
        )
    endif()

    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/MachTargets-debug.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/MachTargets-debug.cmake"
        @ONLY
    )

    install(
        FILES "${CMAKE_BINARY_DIR}/Debug/machd.lib"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    )

    install(
        FILES "${CMAKE_BINARY_DIR}/MachTargets-debug.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Mach"
    )


# Linux

elseif(UNIX AND NOT APPLE)
    set(CPACK_GENERATOR "DEB")

    set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")

    set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Mach")
    set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")

    if(NOT MACH_CLI_EXECUTABLE)
        message(FATAL_ERROR
            "MACH_CLI_EXECUTABLE must be provided when creating the Linux package."
        )
    endif()

    if(NOT EXISTS "${MACH_CLI_EXECUTABLE}")
        message(FATAL_ERROR
            "Mach CLI executable not found: ${MACH_CLI_EXECUTABLE}"
        )
    endif()

    if(NOT MACH_CLI_TEMPLATE_DIR)
        message(FATAL_ERROR
            "MACH_CLI_TEMPLATE_DIR must be provided when creating the Linux package."
        )
    endif()

    if(NOT EXISTS "${MACH_CLI_TEMPLATE_DIR}")
        message(FATAL_ERROR
            "Mach CLI template directory not found: ${MACH_CLI_TEMPLATE_DIR}"
        )
    endif()

    if(NOT MACH_DEBUG_BUILD_DIR)
        message(FATAL_ERROR
            "MACH_DEBUG_BUILD_DIR must be provided when creating the Linux package."
        )
    endif()

    if(NOT EXISTS "${MACH_DEBUG_BUILD_DIR}/libmachd.a")
        message(FATAL_ERROR
            "Mach Debug library not found: ${MACH_DEBUG_BUILD_DIR}/libmachd.a"
        )
    endif()

    install(
        PROGRAMS "${MACH_CLI_EXECUTABLE}"
        DESTINATION "${CMAKE_INSTALL_BINDIR}"
        RENAME "mach"
    )

    install(
        DIRECTORY "${MACH_CLI_TEMPLATE_DIR}/"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/mach/templates"
    )

    install(
        FILES "${MACH_DEBUG_BUILD_DIR}/libmachd.a"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    )

    set(
    MACH_DEBUG_LIBRARY_PATH
    "${CMAKE_INSTALL_LIBDIR}/libmachd.a"
    )

    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/MachTargets-debug.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/MachTargets-debug.cmake"
        @ONLY
    )

    install(
        FILES "${CMAKE_BINARY_DIR}/MachTargets-debug.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Mach"
    )

    install(
        DIRECTORY
            "${CMAKE_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/include/"
        DESTINATION
            "${MACH_DEPENDENCY_INSTALL_DIR}/include"
    )

    install(
        DIRECTORY
            "${CMAKE_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/lib/"
        DESTINATION
            "${MACH_DEPENDENCY_INSTALL_DIR}/lib"
    )

    install(
        DIRECTORY
            "${CMAKE_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/debug/"
        DESTINATION
            "${MACH_DEPENDENCY_INSTALL_DIR}/debug"
    )

    install(
        DIRECTORY
            "${CMAKE_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/share/"
        DESTINATION
            "${MACH_DEPENDENCY_INSTALL_DIR}/share"
    )
endif()

include(CPack)