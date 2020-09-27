project(KubeCore)

get_filename_component(KubeCoreDir ${CMAKE_CURRENT_LIST_FILE} PATH)

set(KubeCoreSources
    ${KubeCoreDir}/Dummy.cpp
    ${KubeCoreDir}/Core.hpp
    ${KubeCoreDir}/Version.hpp
    ${KubeCoreDir}/Hash.hpp
    ${KubeCoreDir}/Assert.hpp
    ${KubeCoreDir}/SafeQueue.hpp
    ${KubeCoreDir}/SafeQueue.ipp
    ${KubeCoreDir}/SPSCQueue.hpp
    ${KubeCoreDir}/SPSCQueue.ipp
    ${KubeCoreDir}/MPMCQueue.hpp
    ${KubeCoreDir}/MPMCQueue.ipp
    ${KubeCoreDir}/HeapArray.hpp
    ${KubeCoreDir}/HeapArray.ipp
    ${KubeCoreDir}/FlatVector.hpp
    ${KubeCoreDir}/FlatVector.ipp
    ${KubeCoreDir}/FlatString.hpp
    ${KubeCoreDir}/FlatString.ipp
)

add_library(${PROJECT_NAME} ${KubeCoreSources})

# target_link_libraries(${PROJECT_NAME}
# PUBLIC
# )

if(${KF_TESTS})
    include(${KubeCoreDir}/Tests/CoreTests.cmake)
endif()

if(${KF_BENCHMARKS})
    include(${KubeCoreDir}/Benchmarks/CoreBenchmarks.cmake)
endif()