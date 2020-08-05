project(KubeCoreBenchmarks)

get_filename_component(KubeCoreBenchmarksDir ${CMAKE_CURRENT_LIST_FILE} PATH)

set(KubeCoreBenchmarksSources
    ${KubeCoreBenchmarksDir}/Main.cpp
    ${KubeCoreBenchmarksDir}/bench_SafeQueue.cpp
    ${KubeCoreBenchmarksDir}/bench_SPSCQueue.cpp
)

add_executable(${CMAKE_PROJECT_NAME} ${KubeCoreBenchmarksSources})

target_link_libraries(${CMAKE_PROJECT_NAME}
PUBLIC
    KubeCore
    benchmark::benchmark
)