#==========================================
#   Chess Engine Tests
#       by Jens W. Langenberg
#==========================================

cmake_minimum_required(VERSION 3.25)

project(Chess.Engine.Tests)
set(CMAKE_CXX_STANDARD 20)

enable_testing()


#==========================================
#   Test Files
#==========================================

set(MoveTest_Dir            source/MoveTests)
set(MultiplayerTest_Dir     source/MultiplayerTests)
set(GameMechanicTest_Dir    source/GameMechanicTests)


set(MoveTest_Files
    ${MoveTest_Dir}/MoveGenerationTests.cpp
    ${MoveTest_Dir}/MoveValidationTests.cpp
    ${MoveTest_Dir}/MoveExecutionTests.cpp
    ${MoveTest_Dir}/CastlingTests.cpp
    ${MoveTest_Dir}/EnPassantTests.cpp
)


set(MultiplayerTest_Files
    ${MultiplayerTest_Dir}/DiscoveryEndpointTests.cpp
    ${MultiplayerTest_Dir}/MultiplayerMessageTests.cpp
    ${MultiplayerTest_Dir}/RemoteCommunicationTests.cpp
    ${MultiplayerTest_Dir}/DiscoveryServiceTests.cpp
    ${MultiplayerTest_Dir}/MultiplayerManagerTests.cpp
    ${MultiplayerTest_Dir}/TCPClientAndServerTests.cpp
    ${MultiplayerTest_Dir}/TCPSessionTests.cpp
    ${MultiplayerTest_Dir}/MultiplayerIntegrationTests.cpp
)


set(GameMechanicTest_Files
    ${GameMechanicTest_Dir}/ChessboardTests.cpp
    ${GameMechanicTest_Dir}/PawnPromotionTests.cpp
    ${GameMechanicTest_Dir}/EndgameTests.cpp
    ${GameMechanicTest_Dir}/UndoMoveTests.cpp
    ${GameMechanicTest_Dir}/MoveHistoryTests.cpp
)


set(Test_Files
    ${MoveTest_Files}
    ${MultiplayerTest_Files}
    ${GameMechanicTest_Files}
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
        ${GOOGLETEST_SOURCE_DIR}/googlemock/include
)

include(GoogleTest)


#==========================================
#   Link Libraries
#==========================================

target_link_libraries(${PROJECT_NAME}
      PRIVATE
        Chess.Engine.Core
        GTest::gtest_main
        GTest::gmock_main
)


#==========================================
#   Discover GTests
#==========================================

if (CMAKE_GENERATOR STREQUAL Xcode)
gtest_discover_tests(${PROJECT_NAME} DISCOVERY_MODE PRE_TEST)
else()
gtest_discover_tests(${PROJECT_NAME})
endif()
