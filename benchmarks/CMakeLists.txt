function(add_mach_bench test_name)
    add_executable("${test_name}"
        ${ARGN}
    )

    target_link_libraries("${test_name}"
        PRIVATE mach
    )
endfunction()

add_mach_bench(
    RoutingBenchmarks
    routing/main.cpp
)

add_mach_bench(
    PipelineBenchmarks
    pipeline/main.cpp
)


target_include_directories(PipelineBenchmarks
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
)

add_mach_bench(
    BindingPerformanceBenchmark
    binding/BindingPerformanceBenchmark.cpp
)

add_mach_bench(
    RequestBenchmarks
    request/RequestBenchmarks.cpp
)
