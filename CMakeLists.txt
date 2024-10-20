cmake_minimum_required(VERSION 3.25)

project(ChessGame VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable folders in Visual Studio solution
set_property(GLOBAL PROPERTY USE_FOLDERS ON)


# Define source and header directories
set(SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
set(PIECE_DIR ${SOURCE_DIR}/Piece)
set(MOVE_DIR ${SOURCE_DIR}/Moves)
set(CONFIG_DIR ${SOURCE_DIR}/ConfigurationManager)
set(BOARD_DIR ${SOURCE_DIR}/Board)
set(PLAYER_DIR ${SOURCE_DIR}/Player)
set(API_DIR ${SOURCE_DIR}/API)


# Include directories
include_directories(    ${PIECE_DIR}
                        ${MOVE_DIR}
                        ${CONFIG_DIR}
                        ${BOARD_DIR}
                        ${PLAYER_DIR}
                        ${API_DIR}
                        ${SOURCE_DIR}
)


set(PIECE_FILES
    ${PIECE_DIR}/ChessPiece.h
    ${PIECE_DIR}/Pawn.h
    ${PIECE_DIR}/Pawn.cpp
    ${PIECE_DIR}/Knight.h
    ${PIECE_DIR}/Knight.cpp
    ${PIECE_DIR}/Bishop.h
    ${PIECE_DIR}/Bishop.cpp
    ${PIECE_DIR}/Rook.h
    ${PIECE_DIR}/Rook.cpp
    ${PIECE_DIR}/Queen.h
    ${PIECE_DIR}/Queen.cpp
    ${PIECE_DIR}/King.h
    ${PIECE_DIR}/King.cpp
)


set(BOARD_FILES
    ${BOARD_DIR}/ChessBoard.h
    ${BOARD_DIR}/ChessBoard.cpp
)

    
set(MOVE_FILES
    ${MOVE_DIR}/Move.h
    ${MOVE_DIR}/MoveHelper.h
    ${MOVE_DIR}/MoveHelper.cpp
    ${MOVE_DIR}/MovementManager.h
    ${MOVE_DIR}/MovementManager.cpp
)


set(CONFIG_FILES
    ${CONFIG_DIR}/GameManager.h
    ${CONFIG_DIR}/GameManager.cpp
)    


set(PLAYER_FILES
    ${PLAYER_DIR}/Player.h
)


set(ALL_FILES
    ${PIECE_FILES}
    ${BOARD_FILES}
    ${MOVE_FILES}
    ${CONFIG_FILES}
    ${PLAYER_FILES}
    ${SOURCE_DIR}/main.cpp
)


# Group source and header files (optional, for IDEs)
source_group("Pieces" FILES ${PIECE_FILES})
source_group("Board" FILES ${BOARD_FILES})
source_group("Moves" FILES ${MOVE_FILES})
source_group("Configuration" FILES ${CONFIG_FILES})
source_group("Player" FILES ${PLAYER_FILES})


# Create the executable target
add_executable(${PROJECT_NAME}
    ${ALL_FILES}
)

# Set output properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "${PROJECT_NAME}"
)

# Add compile definitions (if needed)
target_compile_definitions(${PROJECT_NAME} PUBLIC
    -DENV_DEVELOPMENT
    _CRT_SECURE_NO_WARNINGS
)

# Add compile options
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

# Link libraries 
target_link_libraries(${PROJECT_NAME} PRIVATE
    # Add libraries here
)
