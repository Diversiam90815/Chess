cmake_minimum_required(VERSION 3.25)

project(ChessGame VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable folders in Visual Studio solution
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Define source and header directories
set(SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
set(CHESS_PIECE_DIR ${SOURCE_DIR}/ChessPiece)

# Include directories
include_directories(    ${CHESS_PIECE_DIR}
                        ${SOURCE_DIR}
)


set(SOURCE_FILES
    ${SOURCE_DIR}/main.cpp
    ${CHESS_PIECE_DIR}/ChessPiece.cpp
    ${CHESS_PIECE_DIR}/Pawn.cpp
    ${CHESS_PIECE_DIR}/Knight.cpp
    ${CHESS_PIECE_DIR}/Bishop.cpp
    ${CHESS_PIECE_DIR}/Rook.cpp
    ${CHESS_PIECE_DIR}/Queen.cpp
    ${CHESS_PIECE_DIR}/King.cpp

    ${SOURCE_DIR}/ChessBoard.cpp
)

set(HEADER_FILES
    ${CHESS_PIECE_DIR}/PieceType.h
    ${CHESS_PIECE_DIR}/ChessPiece.h
    ${CHESS_PIECE_DIR}/Pawn.h
    ${CHESS_PIECE_DIR}/Knight.h
    ${CHESS_PIECE_DIR}/Bishop.h
    ${CHESS_PIECE_DIR}/Rook.h
    ${CHESS_PIECE_DIR}/Queen.h
    ${CHESS_PIECE_DIR}/King.h

    ${SOURCE_DIR}/ChessBoard.h
)


# Group source and header files (optional, for IDEs)
source_group("Source Files" FILES ${SOURCE_FILES})
source_group("Header Files" FILES ${HEADER_FILES})

# Create the executable target
add_executable(${PROJECT_NAME}
    ${SOURCE_FILES}
    ${HEADER_FILES} # Optional
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
