function(add_mach_bench bench_name)
    add_executable("${bench_name}"
        ${ARGN}
    )

    target_link_libraries("${bench_name}"
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