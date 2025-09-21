###########################################
#   Chess Engine
#       by Jens W. Langenberg
###########################################

cmake_minimum_required(VERSION 3.25)

set(CHESS_VERSION 1.1.0.978)

project(Chess.Engine VERSION ${CHESS_VERSION} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(CTest)

# Include sub-projects.
add_subdirectory (src)
add_subdirectory (tests)
