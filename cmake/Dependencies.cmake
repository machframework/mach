include(FetchContent)

set(MACH_CAN_INSTALL ON)

find_package(Boost CONFIG QUIET)

if(NOT Boost_FOUND)
    set(MACH_CAN_INSTALL OFF)

    FetchContent_Declare(
        boost
        URL https://archives.boost.io/release/1.90.0/source/boost_1_90_0.tar.gz
    )

    FetchContent_MakeAvailable(boost)

    set(MACH_BOOST_INCLUDE_DIR "${boost_SOURCE_DIR}")
else()
    set(MACH_BOOST_INCLUDE_DIR "${Boost_INCLUDE_DIRS}")
endif()

find_package(nlohmann_json CONFIG QUIET)

if(NOT nlohmann_json_FOUND)
    set(MACH_CAN_INSTALL OFF)

    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.12.0
    )

    FetchContent_MakeAvailable(nlohmann_json)
endif()

find_package(fmt CONFIG QUIET)

if(NOT fmt_FOUND)
    set(MACH_CAN_INSTALL OFF)

    FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 12.1.0
    )

    FetchContent_MakeAvailable(fmt)
endif()

find_package(spdlog CONFIG QUIET)

if(NOT spdlog_FOUND)
    set(MACH_CAN_INSTALL OFF)

    FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.17.0
    )

    FetchContent_MakeAvailable(spdlog)
endif()

if(MACH_BUILD_TESTS)
    find_package(Catch2 3 CONFIG QUIET)

    if(NOT Catch2_FOUND)
        FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG v3.15.3
        )

        FetchContent_MakeAvailable(Catch2)
    endif()
endif()