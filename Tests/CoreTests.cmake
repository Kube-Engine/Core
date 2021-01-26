project(KubeCoreTests)

get_filename_component(KubeCoreTestsDir ${CMAKE_CURRENT_LIST_FILE} PATH)

set(KubeCoreTestsSources
    ${KubeCoreTestsDir}/tests_HeapArray.cpp
    ${KubeCoreTestsDir}/tests_Vector.cpp
    ${KubeCoreTestsDir}/tests_SortedVector.cpp
    ${KubeCoreTestsDir}/tests_String.cpp
    ${KubeCoreTestsDir}/tests_TrivialFunctor.cpp
    ${KubeCoreTestsDir}/tests_Functor.cpp
    ${KubeCoreTestsDir}/tests_Dispatcher.cpp
    ${KubeCoreTestsDir}/tests_SPSCQueue.cpp
    ${KubeCoreTestsDir}/tests_MPMCQueue.cpp
    ${KubeCoreTestsDir}/tests_Literal.cpp
)

add_executable(${CMAKE_PROJECT_NAME} ${KubeCoreTestsSources})

add_test(NAME ${CMAKE_PROJECT_NAME} COMMAND ${CMAKE_PROJECT_NAME})

target_link_libraries(${CMAKE_PROJECT_NAME}
PUBLIC
    KubeCore
    GTest::GTest GTest::Main
)

if(KF_COVERAGE)
    target_compile_options(${PROJECT_NAME} PUBLIC --coverage)
    target_link_options(${PROJECT_NAME} PUBLIC --coverage)
endif()