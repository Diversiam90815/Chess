#==========================================
#   Chess Engine Performance
#       by Jens W. Langenberg
#==========================================

cmake_minimum_required(VERSION 3.25)

project(Chess.Engine.Performance)
set(CMAKE_CXX_STANDARD 20)

enable_testing()


#==========================================
#   Test Files
#==========================================

set(CPUPerformanceTests_Dir    source/CPUPerformanceTests)

set(CPUPerformanceTest_Files
    ${CPUPerformanceTests_Dir}/CPUPerformanceTests.cpp
)


set(Test_Files
    ${CPUPerformanceTest_Files}
)


#==========================================
#   Source Groups (VS)
#==========================================

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Test_Files})


#==========================================
#   Setup Test Project
#==========================================

add_executable(${PROJECT_NAME}
    source/main.cpp
    ${Test_Files}
)


#==========================================
#   Include Directories
#==========================================

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${GOOGLETEST_SOURCE_DIR}/googletest/include
)

include(GoogleTest)


#==========================================
#   Link Libraries
#==========================================

target_link_libraries(${PROJECT_NAME}
      PRIVATE
        Chess.Engine.Core
        GTest::gtest_main
)


#==========================================
#   Discover GTests
#==========================================

if (CMAKE_GENERATOR STREQUAL Xcode)
gtest_discover_tests(${PROJECT_NAME} DISCOVERY_MODE PRE_TEST)
else()
gtest_discover_tests(${PROJECT_NAME})
endif()
