add_executable(mach_hello_world
    hello_world/main.cpp
)

target_link_libraries(mach_hello_world
    PRIVATE mach
)