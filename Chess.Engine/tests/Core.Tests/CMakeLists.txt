set(MoveTest_Dir            source/MoveTests)
set(MultiplayerTest_Dir     source/MultiplayerTests)
set(BoardTest_Dir           source/BoardTests)
set(GameMechanicTest_Dir    source/GameMechanicTests)
set(PlayerTest_Dir          source/PlayerTests)

set (Test_Dir						${CMAKE_CURRENT_SOURCE_DIR}/source)

set(MoveTest_Files
    ${MoveTest_Dir}/MoveTests.cpp
    ${MoveTest_Dir}/MoveListTests.cpp
    ${MoveTest_Dir}/MoveIntentTests.cpp
    ${MoveTest_Dir}/MoveGenerationTests.cpp
    ${MoveTest_Dir}/MoveValidationTests.cpp
    ${MoveTest_Dir}/MoveExecutionTests.cpp
    ${MoveTest_Dir}/CastlingTests.cpp
    ${MoveTest_Dir}/EnPassantTests.cpp
    ${MoveTest_Dir}/MoveHistoryTests.cpp
    ${MoveTest_Dir}/MoveNotationTests.cpp
    ${MoveTest_Dir}/MoveEvaluationTests.cpp
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
    ${GameMechanicTest_Dir}/PawnPromotionTests.cpp
    ${GameMechanicTest_Dir}/EndgameTests.cpp
    ${GameMechanicTest_Dir}/UndoMoveTests.cpp
)


set(PlayerTest_Files
    ${PlayerTest_Dir}/PlayerTests.cpp
    ${PlayerTest_Dir}/CPUPlayerTests.cpp
)


set(BoardTest_Files
    ${BoardTest_Dir}/ChessboardTests.cpp
)


set(Test_Files
    ${MoveTest_Files}
    ${BoardTest_Files}
    ${PlayerTest_Files}
    ${MultiplayerTest_Files}
    ${GameMechanicTest_Files}
)

source_group(TREE ${Test_Dir} FILES ${Test_Files})


#==========================================
#   Setup Test Project
#==========================================

add_executable(Engine.Core.Tests ${Test_Files})

target_link_libraries(Engine.Core.Tests PRIVATE Chess.Engine.Core)

AddTests(Engine.Core.Tests)
