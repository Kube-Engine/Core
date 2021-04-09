project(KubeCore)

get_filename_component(KubeCoreDir ${CMAKE_CURRENT_LIST_FILE} PATH)

set(KubeCoreSources
    ${KubeCoreDir}/AllocatedFlatString.hpp
    ${KubeCoreDir}/AllocatedFlatVector.hpp
    ${KubeCoreDir}/AllocatedFlatVectorBase.hpp
    ${KubeCoreDir}/AllocatedSmallString.hpp
    ${KubeCoreDir}/AllocatedSmallVector.hpp
    ${KubeCoreDir}/AllocatedSmallVectorBase.hpp
    ${KubeCoreDir}/AllocatedString.hpp
    ${KubeCoreDir}/AllocatedVector.hpp
    ${KubeCoreDir}/AllocatedVectorBase.hpp
    ${KubeCoreDir}/Assert.hpp
    ${KubeCoreDir}/Dispatcher.hpp
    ${KubeCoreDir}/DispatcherDetails.hpp
    ${KubeCoreDir}/FlatString.hpp
    ${KubeCoreDir}/FlatVector.hpp
    ${KubeCoreDir}/FlatVectorBase.hpp
    ${KubeCoreDir}/FlatVectorBase.ipp
    ${KubeCoreDir}/Functor.hpp
    ${KubeCoreDir}/Hash.hpp
    ${KubeCoreDir}/HeapArray.hpp
    ${KubeCoreDir}/HeapArray.ipp
    ${KubeCoreDir}/MacroUtils.hpp
    ${KubeCoreDir}/MPMCQueue.hpp
    ${KubeCoreDir}/MPMCQueue.ipp
    ${KubeCoreDir}/SmallString.hpp
    ${KubeCoreDir}/SmallVector.hpp
    ${KubeCoreDir}/SmallVectorBase.hpp
    ${KubeCoreDir}/SmallVectorBase.ipp
    ${KubeCoreDir}/SortedAllocatedFlatVector.hpp
    ${KubeCoreDir}/SortedAllocatedSmallVector.hpp
    ${KubeCoreDir}/SortedAllocatedVector.hpp
    ${KubeCoreDir}/SortedFlatVector.hpp
    ${KubeCoreDir}/SortedSmallVector.hpp
    ${KubeCoreDir}/SortedVector.hpp
    ${KubeCoreDir}/SortedVectorDetails.hpp
    ${KubeCoreDir}/SortedVectorDetails.ipp
    ${KubeCoreDir}/SPSCQueue.hpp
    ${KubeCoreDir}/SPSCQueue.ipp
    ${KubeCoreDir}/String.hpp
    ${KubeCoreDir}/StringDetails.hpp
    ${KubeCoreDir}/StringDetails.ipp
    ${KubeCoreDir}/StringLiteral.hpp
    ${KubeCoreDir}/StringUtils.hpp
    ${KubeCoreDir}/TrivialDispatcher.hpp
    ${KubeCoreDir}/TrivialFunctor.hpp
    ${KubeCoreDir}/Utils.hpp
    ${KubeCoreDir}/Utils.ipp
    ${KubeCoreDir}/Vector.hpp
    ${KubeCoreDir}/VectorBase.hpp
    ${KubeCoreDir}/VectorBase.ipp
    ${KubeCoreDir}/VectorDetails.hpp
    ${KubeCoreDir}/VectorDetails.ipp
    ${KubeCoreDir}/Version.hpp
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