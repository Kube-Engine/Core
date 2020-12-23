project(KubeCore)

get_filename_component(KubeCoreDir ${CMAKE_CURRENT_LIST_FILE} PATH)

set(KubeCoreSources
    ${KubeCoreDir}/MacroUtils.hpp
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
    ${KubeCoreDir}/VectorDetails.hpp
    ${KubeCoreDir}/Vector.hpp
    ${KubeCoreDir}/Vector.ipp
    ${KubeCoreDir}/SmallVector.hpp
    ${KubeCoreDir}/SmallVector.ipp
    ${KubeCoreDir}/TrivialFunctor.hpp
    ${KubeCoreDir}/Functor.hpp
    ${KubeCoreDir}/DispatcherDetails.hpp
    ${KubeCoreDir}/TrivialDispatcher.hpp
)

add_library(${PROJECT_NAME} INTERFACE) #${KubeCoreSources})

# target_link_libraries(${PROJECT_NAME}
# PUBLIC
# )

if(${KF_TESTS})
    include(${KubeCoreDir}/Tests/CoreTests.cmake)
endif()

if(${KF_BENCHMARKS})
    include(${KubeCoreDir}/Benchmarks/CoreBenchmarks.cmake)
endif()