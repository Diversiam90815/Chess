#==========================================
#   Chess Engine
#       by Jens W. Langenberg
#==========================================

cmake_minimum_required(VERSION 3.25)

set(CHESS_VERSION 1.0.0.502)

project(Chess.Engine VERSION ${CHESS_VERSION} LANGUAGES CXX)


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
	
set (PIECE_DIR              ${SOURCE_DIR}/Piece)
set (MOVE_DIR               ${SOURCE_DIR}/Moves)
set (GAME_MANAGER_DIR       ${SOURCE_DIR}/GameManager)
set (BOARD_DIR              ${SOURCE_DIR}/Board)
set (PLAYER_DIR             ${SOURCE_DIR}/Player)
set (API_DIR                ${SOURCE_DIR}/API)
set (HELPER_DIR             ${SOURCE_DIR}/Helper)
set (FILEMANAGER_DIR        ${SOURCE_DIR}/FileManager)
set (LOGGER_DIR             ${SOURCE_DIR}/Logger)
set (USERSETTINGS_DIR       ${SOURCE_DIR}/UserSettings)
set (COMMUNICATION_DIR      ${SOURCE_DIR}/Communication)
set (OBSERVER_DIR           ${SOURCE_DIR}/Observer)
set (NETWORK_DIR            ${SOURCE_DIR}/Network)
set (MULTIPLAYER_DIR        ${SOURCE_DIR}/Multiplayer)
set (STATEMACHINE_DIR       ${SOURCE_DIR}/StateMachine)


#==========================================
#   INCLUDE DIRECTORIES
#==========================================

set(ALL_PROJECT_DIRS 
			${PIECE_DIR}
			${MOVE_DIR}
			${GAME_MANAGER_DIR}
			${BOARD_DIR}
			${PLAYER_DIR}
			${API_DIR}
			${HELPER_DIR}
			${LOGGER_DIR}
			${FILEMANAGER_DIR}
			${SOURCE_DIR}
			${USERSETTINGS_DIR}
			${COMMUNICATION_DIR}
			${OBSERVER_DIR}
			${NETWORK_DIR}
			${MULTIPLAYER_DIR}
			${STATEMACHINE_DIR}
)

include_directories(${ALL_PROJECT_DIRS})


#==========================================
#   SET THE FILES
#==========================================

set(API_FILES
	${API_DIR}/EngineAPI.h    ${API_DIR}/EngineAPI.cpp
	${API_DIR}/EngineAPIDefines.h
)

set(STATEMACHINE_FILES
	${STATEMACHINE_DIR}/StateMachine.h	${STATEMACHINE_DIR}/StateMachine.cpp
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
	${BOARD_DIR}/ChessBoard.h    ${BOARD_DIR}/ChessBoard.cpp
)
	
set(MOVE_FILES
	${MOVE_DIR}/MoveType.h
	${MOVE_DIR}/Move.h
	${MOVE_DIR}/MoveSerialization.h
	${MOVE_DIR}/MoveHelper.h                ${MOVE_DIR}/MoveHelper.cpp
	${MOVE_DIR}/MoveGeneration.h            ${MOVE_DIR}/MoveGeneration.cpp
	${MOVE_DIR}/MoveValidation.h            ${MOVE_DIR}/MoveValidation.cpp
	${MOVE_DIR}/MoveExecution.h             ${MOVE_DIR}/MoveExecution.cpp
	${MOVE_DIR}/MoveNotationHelper.h        ${MOVE_DIR}/MoveNotationHelper.cpp
)

set(GAME_MANAGER_FILES
	${GAME_MANAGER_DIR}/GameManager.h    ${GAME_MANAGER_DIR}/GameManager.cpp
)

set(PLAYER_FILES
	${PLAYER_DIR}/Player.h      ${PLAYER_DIR}/Player.cpp
)

set(HELPER_FILES
	${HELPER_DIR}/JsonConversion.h
	${HELPER_DIR}/Parameters.h
	${HELPER_DIR}/ThreadBase.h
	${HELPER_DIR}/Conversion.h
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

set(OBSERVER_FILES
	${OBSERVER_DIR}/IObserver.h
	${OBSERVER_DIR}/IObservable.h
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
	${BOARD_FILES}
	${MOVE_FILES}
	${GAME_MANAGER_FILES}
	${PLAYER_FILES}
	${HELPER_FILES}
	${LOGGER_FILES}
	${FILEMANAGER_FILES}
	${USERSETTINGS_FILES}
	${COMMUNICATION_FILES}
	${OBSERVER_FILES}
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
add_subdirectory(${JSON_DIR})


#==========================================
#   CREATE TARGET
#==========================================

add_library(${PROJECT_NAME}.Core STATIC   
  ${ALL_FILES}
)

add_library(${PROJECT_NAME} SHARED
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

target_compile_definitions(${PROJECT_NAME}.Core PUBLIC
	-DENV_DEVELOPMENT
	_CRT_SECURE_NO_WARNINGS

	_WINDOWS
	WIN32_LEAN_AND_MEAN
	_WINSOCK_DEPRECATED_NO_WARNING
	_WIN32_WINNT=0x0A00

)

target_compile_definitions(${PROJECT_NAME} PUBLIC
	-DENV_DEVELOPMENT
	_CRT_SECURE_NO_WARNINGS

	_WINDOWS
	WIN32_LEAN_AND_MEAN
	_WINSOCK_DEPRECATED_NO_WARNING
	_WIN32_WINNT=0x0A00

)


#==========================================
#   COMPILE OPTIONS
#==========================================

target_compile_options(${PROJECT_NAME}.Core PRIVATE
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

target_include_directories(${PROJECT_NAME}.Core PUBLIC ${Include_Dirs})
target_include_directories(${PROJECT_NAME} PUBLIC ${Include_Dirs})


#==========================================
#   LINK LIBRARIES
#==========================================

set(PROJECT_LIBS Logger)

target_link_libraries(${PROJECT_NAME}.Core PUBLIC ${PROJECT_LIBS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${PROJECT_LIBS})

