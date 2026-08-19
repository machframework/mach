function(add_mach_example example_name)
    add_executable("${example_name}"
        ${ARGN}
    )

    target_link_libraries("${example_name}"
        PRIVATE mach
    )
endfunction()

add_mach_example(
    mach_hello_world
    hello_world/main.cpp
)

add_mach_example(
    mach_controllers
    controllers/main.cpp
)

add_mach_example(
    mach_middleware
    middleware/main.cpp
)

add_mach_example(
    mach_body_binding
    body_binding/main.cpp
)

add_mach_example(
    mach_dependency_injection
    dependency_injection/main.cpp
)

add_mach_example(
    mach_validation
    validation/main.cpp
)

add_mach_example(
    mach_cookies
    cookies/main.cpp
)
