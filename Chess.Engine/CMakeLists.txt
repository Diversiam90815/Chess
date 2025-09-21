###########################################
#   Chess Engine
#       by Jens W. Langenberg
###########################################

cmake_minimum_required(VERSION 3.25)

set(CHESS_VERSION 1.1.0.974)

project(Chess.Engine VERSION ${CHESS_VERSION} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

## Setting the lib directory for project included packages
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)

include(CTest)

# Include sub-projects.
add_subdirectory (src)
add_subdirectory (tests)
