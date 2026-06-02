cmake_minimum_required(VERSION 3.25)

project(Mach VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Boost REQUIRED)

add_library(mach
    src/server/BeastListener.cpp
    src/server/BeastSession.cpp
    src/logging/Logging.cpp
    src/server/Server.cpp
)

target_include_directories(mach
    PUBLIC
        include
    PRIVATE
        src
)

target_link_libraries(mach
    PRIVATE
        Boost::headers
)