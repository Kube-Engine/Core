project(KubeCoreTests)

get_filename_component(KubeCoreTestsDir ${CMAKE_CURRENT_LIST_FILE} PATH)

set(KubeCoreTestsSources
   ${KubeCoreTestsDir}/tests_SPSCQueue.cpp
   ${KubeCoreTestsDir}/tests_MPMCQueue.cpp
   ${KubeCoreTestsDir}/tests_HeapArray.cpp
    ${KubeCoreTestsDir}/tests_FlatVector.cpp
   ${KubeCoreTestsDir}/tests_FlatString.cpp
)

add_executable(${CMAKE_PROJECT_NAME} ${KubeCoreTestsSources})

add_test(NAME ${CMAKE_PROJECT_NAME} COMMAND ${CMAKE_PROJECT_NAME})

target_link_libraries(${CMAKE_PROJECT_NAME}
PUBLIC
    KubeCore
    GTest::GTest GTest::Main
)