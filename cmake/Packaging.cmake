set(CPACK_PACKAGE_NAME "Mach")
set(CPACK_PACKAGE_VENDOR "Mach")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")

set(CPACK_PACKAGE_INSTALL_DIRECTORY "Mach")

if(WIN32)
    set(CPACK_GENERATOR "WIX")
    set(CPACK_WIX_ROOT "C:/Program Files (x86)/WiX Toolset v3.14")

    set(
    CPACK_WIX_UPGRADE_GUID
    "97A0CA68-0215-4513-A8B3-F4DBB6BB6977"
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
endif()

include(CPack)