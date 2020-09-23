project(KubeCoreTests)

get_filename_component(KubeCoreTestsDir ${CMAKE_CURRENT_LIST_FILE} PATH)

set(KubeCoreTestsSources
    ${KubeCoreTestsDir}/tests_SafeQueue.cpp
    ${KubeCoreTestsDir}/tests_SafeAccessTable.cpp
    ${KubeCoreTestsDir}/tests_SPSCQueue.cpp
    ${KubeCoreTestsDir}/tests_MPMCQueue.cpp
    ${KubeCoreTestsDir}/tests_FlatVector.cpp
    ${KubeCoreTestsDir}/tests_HeapArray.cpp
)

add_executable(${CMAKE_PROJECT_NAME} ${KubeCoreTestsSources})

add_test(NAME ${CMAKE_PROJECT_NAME} COMMAND ${CMAKE_PROJECT_NAME})

target_link_libraries(${CMAKE_PROJECT_NAME}
PUBLIC
    KubeCore
    GTest::GTest GTest::Main
)