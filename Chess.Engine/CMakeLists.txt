###########################################
#   Chess Engine
#       by Jens W. Langenberg
###########################################

cmake_minimum_required(VERSION 3.25)

project ("Chess")

## Setting the lib directory for project included packages
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)

## Include the CPM
include(cmake/cpm.cmake)

CPMAddPackage(
        NAME GOOGLETEST
        GITHUB_REPOSITORY google/googletest
        VERSION 1.15.2
        SOURCE_DIR ${LIB_DIR}/googletest
        OPTIONS
        "INSTALL_GTEST OFF"
        "gtest_force_shared_crt ON"
        )
        
enable_testing()

# Include sub-projects.
add_subdirectory (Chess.Engine)
add_subdirectory (Chess.Engine.Tests)