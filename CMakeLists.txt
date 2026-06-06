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
    src/http/Request.cpp
    src/http/Response.cpp
    src/adapter/inbound/BeastRequestAdapter.cpp
    src/adapter/outbound/BeastResponseAdapter.cpp
    src/application/Runtime.cpp
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

add_executable(mach_hello_world
 "src/http/Request.cpp"    examples/hello_world/main.cpp
   "src/http/Response.cpp" "src/adapter/inbound/BeastRequestAdapter.cpp" "src/adapter/outbound/BeastResponseAdapter.cpp" "src/application/Runtime.cpp")

target_link_libraries(mach_hello_world
    PRIVATE mach
)