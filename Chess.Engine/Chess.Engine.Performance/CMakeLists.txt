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

set(Perf_Dir            source/PerfFiles)
set(Perf_Helper_Dir     source/PerfHelper)

set(Perf_Files
    ${Perf_Dir}/CPUPerformanceTests.cpp
    ${Perf_Dir}/MoveGenerationPerformanceTests.cpp
    ${Perf_Dir}/MoveEvaluationPerformanceTests.cpp
    ${Perf_Dir}/CPUPlayerPerformanceTests.cpp
    ${Perf_Dir}/PositionalEvaluationPerformanceTests.cpp
    ${Perf_Dir}/LightChessBoardPerformanceTests.cpp
)

set(Perf_Helper_Files
    ${Perf_Helper_Dir}/PerformanceJSONHelper.h
    ${Perf_Helper_Dir}/PerfResults.h
)


#==========================================
#   Source Groups (VS)
#==========================================

source_group("Perf Files"       FILES ${Perf_Files})
source_group("Helper Files"     FILES ${Perf_Helper_Files})


#==========================================
#   Setup Test Project
#==========================================

add_executable(${PROJECT_NAME}
    source/main.cpp
    ${Perf_Files}
    ${Perf_Helper_Files}
)


#==========================================
#   Include Directories
#==========================================

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${GOOGLETEST_SOURCE_DIR}/googletest/include
        ${Perf_Files}
        ${Perf_Helper_Files}
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
