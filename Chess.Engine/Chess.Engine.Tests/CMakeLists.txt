###########################################
#   Chess Engine Tests
#       by Jens W. Langenberg
###########################################

cmake_minimum_required(VERSION 3.25)

project(Chess.Engine.Tests)

enable_testing()


add_executable(${PROJECT_NAME}
    source/main.cpp
    source/MovesTests.cpp
    source/ChessboardTests.cpp
)


target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${GOOGLETEST_SOURCE_DIR}/googletest/include
)


target_link_libraries(${PROJECT_NAME}
      PRIVATE
        Chess.Engine        
        GTest::gtest_main
        )

include(GoogleTest)

## Discover GTests
if (CMAKE_GENERATOR STREQUAL Xcode)
gtest_discover_tests(${PROJECT_NAME} DISCOVERY_MODE PRE_TEST)
else()
gtest_discover_tests(${PROJECT_NAME})
endif()

