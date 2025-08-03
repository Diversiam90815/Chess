#==========================================
#   Chess Engine
#       by Jens W. Langenberg
#==========================================

cmake_minimum_required(VERSION 3.25)

set(CHESS_VERSION 1.1.0.784)

project(Chess.Engine.Core VERSION ${CHESS_VERSION} LANGUAGES CXX)


#==========================================
#   CPP AND PROJECT STANDARDS
#==========================================

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)


#==========================================
#   PROJECT DIRECTORIES
#==========================================

set (SOURCE_DIR             ${PROJECT_SOURCE_DIR}/src)
set (SUBMODULES_DIR         ${PROJECT_SOURCE_DIR}/submodule)
	
set (API_DIR				${SOURCE_DIR}/API)
set (PIECE_DIR              ${SOURCE_DIR}/Piece)
set (MOVE_DIR               ${SOURCE_DIR}/Moves)
set (GAME_MANAGER_DIR       ${SOURCE_DIR}/GameManager)
set (GAME_ENGINE_DIR		${SOURCE_DIR}/GameEngine)
set (BOARD_DIR              ${SOURCE_DIR}/Board)
set (PLAYER_DIR             ${SOURCE_DIR}/Player)
set (HELPER_DIR             ${SOURCE_DIR}/Helper)
set (FILEMANAGER_DIR        ${SOURCE_DIR}/FileManager)
set (LOGGER_DIR             ${SOURCE_DIR}/Logger)
set (USERSETTINGS_DIR       ${SOURCE_DIR}/UserSettings)
set (COMMUNICATION_DIR      ${SOURCE_DIR}/Communication)
set (NETWORK_DIR            ${SOURCE_DIR}/Network)
set (MULTIPLAYER_DIR        ${SOURCE_DIR}/Multiplayer)
set (STATEMACHINE_DIR       ${SOURCE_DIR}/StateMachine)


#==========================================
#   INCLUDE DIRECTORIES
#==========================================

set(ALL_PROJECT_DIRS 
			${API_DIR}
			${PIECE_DIR}
			${MOVE_DIR}
			${GAME_MANAGER_DIR}
			${GAME_ENGINE_DIR}
			${BOARD_DIR}
			${PLAYER_DIR}
			${HELPER_DIR}
			${LOGGER_DIR}
			${FILEMANAGER_DIR}
			${SOURCE_DIR}
			${USERSETTINGS_DIR}
			${COMMUNICATION_DIR}
			${NETWORK_DIR}
			${MULTIPLAYER_DIR}
			${STATEMACHINE_DIR}
)

include_directories(${ALL_PROJECT_DIRS})


#==========================================
#   SET THE FILES
#==========================================

set(API_FILES
	${API_DIR}/EngineAPIDefines.h
)

set(STATEMACHINE_FILES
	${STATEMACHINE_DIR}/StateMachine.h	${STATEMACHINE_DIR}/StateMachine.cpp
)

set(GAMEENGINE_FILES
	${GAME_ENGINE_DIR}/GameEngine.h	${GAME_ENGINE_DIR}/GameEngine.cpp
)

set(PIECE_FILES
	${PIECE_DIR}/ChessPiece.h   ${PIECE_DIR}/ChessPiece.cpp
	${PIECE_DIR}/Pawn.h         ${PIECE_DIR}/Pawn.cpp
	${PIECE_DIR}/Knight.h       ${PIECE_DIR}/Knight.cpp
	${PIECE_DIR}/Bishop.h       ${PIECE_DIR}/Bishop.cpp
	${PIECE_DIR}/Rook.h         ${PIECE_DIR}/Rook.cpp
	${PIECE_DIR}/Queen.h        ${PIECE_DIR}/Queen.cpp
	${PIECE_DIR}/King.h         ${PIECE_DIR}/King.cpp
)

set(BOARD_FILES
	${BOARD_DIR}/ChessBoard.h			${BOARD_DIR}/ChessBoard.cpp
	${BOARD_DIR}/LightChessBoard.h		${BOARD_DIR}/LightChessBoard.cpp
)
	
set(MOVE_FILES
	${MOVE_DIR}/MoveType.h
	${MOVE_DIR}/Move.h
	${MOVE_DIR}/MoveHelper.h					${MOVE_DIR}/MoveHelper.cpp
	${MOVE_DIR}/generation/MoveGeneration.h		${MOVE_DIR}/Generation/MoveGeneration.cpp
	${MOVE_DIR}/Validation/MoveValidation.h		${MOVE_DIR}/Validation/MoveValidation.cpp
	${MOVE_DIR}/Execution/MoveExecution.h		${MOVE_DIR}/Execution/MoveExecution.cpp
	${MOVE_DIR}/Evaluation/MoveEvaluation.h		${MOVE_DIR}/Evaluation/MoveEvaluation.cpp
	${MOVE_DIR}/Notation/MoveNotation.h			${MOVE_DIR}/Notation/MoveNotation.cpp
)

set(GAME_MANAGER_FILES
	${GAME_MANAGER_DIR}/GameManager.h    ${GAME_MANAGER_DIR}/GameManager.cpp
)

set(PLAYER_FILES
	${PLAYER_DIR}/Player.h      	${PLAYER_DIR}/Player.cpp
	${PLAYER_DIR}/PlayerName.h      ${PLAYER_DIR}/PlayerName.cpp
	${PLAYER_DIR}/CPUPlayer.h      	${PLAYER_DIR}/CPUPlayer.cpp
)

set(HELPER_FILES
	${HELPER_DIR}/JsonConversion.h
	${HELPER_DIR}/Parameters.h
	${HELPER_DIR}/ThreadBase.h
	${HELPER_DIR}/Conversion.h
	${HELPER_DIR}/IObserver.h
	${HELPER_DIR}/IObservable.h
)

set(LOGGER_FILES
	${LOGGER_DIR}/Logging.h             ${LOGGER_DIR}/Logging.cpp
	${LOGGER_DIR}/LoggingHelper.h       ${LOGGER_DIR}/LoggingHelper.cpp
)

set(FILEMANAGER_FILES
	${FILEMANAGER_DIR}/FileManager.h    ${FILEMANAGER_DIR}/FileManager.cpp
)

set(USERSETTINGS_FILES
	${USERSETTINGS_DIR}/UserSettings.h    ${USERSETTINGS_DIR}/UserSettings.cpp
)

set(NETWORK_FILES
	${NETWORK_DIR}/NetworkAdapter.h
	${NETWORK_DIR}/NetworkManager.h         ${NETWORK_DIR}/NetworkManager.cpp
	${NETWORK_DIR}/NetworkInformation.h     ${NETWORK_DIR}/NetworkInformation.cpp
)

set(COMMUNICATION_FILES
	${COMMUNICATION_DIR}/UICommunication.h    ${COMMUNICATION_DIR}/UICommunication.cpp
)

set(MULTIPLAYER_FILES
	${MULTIPLAYER_DIR}/ConnectionStatus.h
	${MULTIPLAYER_DIR}/Discovery/DiscoveryEndpoint.h
	${MULTIPLAYER_DIR}/RemoteMessaging/MultiplayerMessageStruct.h
	${MULTIPLAYER_DIR}/RemoteMessaging/CommunicationThreads.h		${MULTIPLAYER_DIR}/RemoteMessaging/CommunicationThreads.cpp
	${MULTIPLAYER_DIR}/MultiplayerManager.h							${MULTIPLAYER_DIR}/MultiplayerManager.cpp
	${MULTIPLAYER_DIR}/Discovery/DiscoveryService.h					${MULTIPLAYER_DIR}/Discovery/DiscoveryService.cpp
	${MULTIPLAYER_DIR}/TCPConnection/TCPInterfaces.h
	${MULTIPLAYER_DIR}/TCPConnection/TCPSession.h					${MULTIPLAYER_DIR}/TCPConnection/TCPSession.cpp
	${MULTIPLAYER_DIR}/TCPConnection/TCPServer.h					${MULTIPLAYER_DIR}/TCPConnection/TCPServer.cpp
	${MULTIPLAYER_DIR}/TCPConnection/TCPClient.h					${MULTIPLAYER_DIR}/TCPConnection/TCPClient.cpp
	${MULTIPLAYER_DIR}/RemoteMessaging/RemoteSender.h				${MULTIPLAYER_DIR}/RemoteMessaging/RemoteSender.cpp
	${MULTIPLAYER_DIR}/RemoteMessaging/RemoteReceiver.h				${MULTIPLAYER_DIR}/RemoteMessaging/RemoteReceiver.cpp
	${MULTIPLAYER_DIR}/RemoteMessaging/RemoteCommunication.h		${MULTIPLAYER_DIR}/RemoteMessaging/RemoteCommunication.cpp
)
	

set(ALL_FILES
	${API_FILES}
	${PIECE_FILES}
	${GAMEENGINE_FILES}
	${BOARD_FILES}
	${MOVE_FILES}
	${GAME_MANAGER_FILES}
	${PLAYER_FILES}
	${HELPER_FILES}
	${LOGGER_FILES}
	${FILEMANAGER_FILES}
	${USERSETTINGS_FILES}
	${COMMUNICATION_FILES}
	${NETWORK_FILES}
	${MULTIPLAYER_FILES}
	${STATEMACHINE_FILES}
)


#==========================================
#   SOURCE GROUPS
#==========================================

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${ALL_FILES})


#==========================================
#   SUBMODULES
#==========================================

# Logger

add_subdirectory(${SUBMODULES_DIR}/Logger)
set(LOGGER_INCLUDE_FILE   ${SUBMODULES_DIR}/Logger/include)


# Asio

set(ASIO_ROOT ${SUBMODULES_DIR}/asio-standalone/asio)
set(Asio_INCLUDE_DIR ${ASIO_ROOT}/include)


# NLOHMANN JSON

set(JSON_DIR ${SUBMODULES_DIR}/json/)
set(JSON_SINGLE_FILE ${JSON_DIR}/single_include/nlohmann)


#==========================================
#   CREATE TARGET
#==========================================

add_library(${PROJECT_NAME} STATIC   
  ${ALL_FILES}
)


#==========================================
#   OUTPUT PROPERTIES
#==========================================

set_target_properties(${PROJECT_NAME} PROPERTIES
	OUTPUT_NAME "${PROJECT_NAME}"
)


#==========================================
#   COMPILE DEFINITIONS
#==========================================

target_compile_definitions(${PROJECT_NAME} PUBLIC
	-DENV_DEVELOPMENT
	_CRT_SECURE_NO_WARNINGS
	_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING

	_WINDOWS
	WIN32_LEAN_AND_MEAN
	_WINSOCK_DEPRECATED_NO_WARNING
	_WIN32_WINNT=0x0A00
)


#==========================================
#   COMPILE OPTIONS
#==========================================

target_compile_options(${PROJECT_NAME} PRIVATE
	$<$<CONFIG:Release>:
		/Oi
		/Gy
	>
	$<$<CONFIG:Debug>:
		/Od
		/Z7
	>
	/sdl
	/MP
	/WX-        # Don't treat warnings as errors
)


#==========================================
#   TARGET INCLUDE DIRECTORIES
#==========================================

set(Include_Dirs 
		${PROJECT_BINARY_DIR}
		${ALL_PROJECT_DIRS}
		${LOGGER_INCLUDE_FILE}
		${JSON_SINGLE_FILE}
		${Asio_INCLUDE_DIR}
)

target_include_directories(${PROJECT_NAME} PUBLIC ${Include_Dirs})


#==========================================
#   LINK LIBRARIES
#==========================================

set(PROJECT_LIBS Logger)

target_link_libraries(${PROJECT_NAME} PUBLIC ${PROJECT_LIBS})

