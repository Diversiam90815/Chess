cmake_minimum_required(VERSION 3.25)

project(ChessGame VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable folders in Visual Studio solution
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Define source and header directories
set(SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
set(HEADER_DIR ${PROJECT_SOURCE_DIR}/include)

# Include directories
include_directories(${HEADER_DIR})

# Collect all source files
set(SOURCE_FILES
    ${SOURCE_DIR}/main.cpp
    #${SOURCE_DIR}/ChessPiece.cpp
    #${SOURCE_DIR}/Pawn.cpp
    #${SOURCE_DIR}/Knight.cpp
    #${SOURCE_DIR}/Bishop.cpp
    #${SOURCE_DIR}/Rook.cpp
    #${SOURCE_DIR}/Queen.cpp
    #${SOURCE_DIR}/King.cpp
    #${SOURCE_DIR}/ChessBoard.cpp
)

# Collect all header files (optional, for IDEs)
set(HEADER_FILES
    #${HEADER_DIR}/PieceType.h
    #${HEADER_DIR}/ChessPiece.h
    #${HEADER_DIR}/Pawn.h
    #${HEADER_DIR}/Knight.h
    #${HEADER_DIR}/Bishop.h
    #${HEADER_DIR}/Rook.h
    #${HEADER_DIR}/Queen.h
    #${HEADER_DIR}/King.h
    #${HEADER_DIR}/ChessBoard.h
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
