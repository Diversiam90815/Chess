###########################################
#   Chess Engine Tests
#       by Jens W. Langenberg
###########################################

cmake_minimum_required(VERSION 3.25)

project(Chess.Engine.Tests)
set(CMAKE_CXX_STANDARD 20)

enable_testing()



set(MoveTest_Dir source/MoveTests)
set(HelperFiles_Dir source/Helper)


set(MoveTest_Files
    ${MoveTest_Dir}/MoveGenerationTests.cpp
    ${MoveTest_Dir}/MoveValidationTests.cpp
    ${MoveTest_Dir}/MoveExecutionTests.cpp
)


set(Test_Files
    ${MoveTest_Files}
    source/ChessboardTests.cpp
)

add_executable(${PROJECT_NAME}
    source/main.cpp
    ${Test_Files}
)


target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${GOOGLETEST_SOURCE_DIR}/googletest/include
        ${HelperFiles_Dir}
)


target_link_libraries(${PROJECT_NAME}
      PRIVATE
        Chess.Engine.Core
        GTest::gtest_main
        )


include(GoogleTest)


source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Test_Files})


## Discover GTests
if (CMAKE_GENERATOR STREQUAL Xcode)
gtest_discover_tests(${PROJECT_NAME} DISCOVERY_MODE PRE_TEST)
else()
gtest_discover_tests(${PROJECT_NAME})
endif()

