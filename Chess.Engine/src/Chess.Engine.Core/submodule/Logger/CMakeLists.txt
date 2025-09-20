cmake_minimum_required(VERSION 3.25)

set(LOGGER_VERSION 1.3.81)

project(Logger LANGUAGES CXX VERSION ${LOGGER_VERSION})

set(CMAKE_INSTALL_PREFIX ${PROJECT_BINARY_DIR})
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

## Setting the lib directory for project included packages
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)

## Include the CPM (Package Manager used by CMake)
if (NOT COMMAND CPMAddPackage)
  if (EXISTS "${CMAKE_SOURCE_DIR}/cmake/cpm.cmake")
    include("${CMAKE_SOURCE_DIR}/cmake/cpm.cmake")
  elseif (EXISTS "${CMAKE_CURRENT_LIST_DIR}/cmake/cpm.cmake")
    include("${CMAKE_CURRENT_LIST_DIR}/cmake/cpm.cmake")
  else()
    message(FATAL_ERROR "CPM.cmake not found and CPMAddPackage is not available.")
  endif()
endif()

include(cpm)

#-------------------------------------
#       Thrid Library Packages
#-------------------------------------

#       SPDLOG
CPMAddPackage(
    NAME spdlog
    GITHUB_REPOSITORY gabime/spdlog
    VERSION 1.15.3
    SOURCE_DIR ${LIB_DIR}/spdlog
)

#       FMT
CPMAddPackage(
    NAME fmt
    VERSION 11.2.0
    GIT_TAG 11.2.0
    GITHUB_REPOSITORY fmtlib/fmt
    SOURCE_DIR ${LIB_DIR}/fmt
)

#       NLOHMANN JSON
CPMAddPackage(
    NAME nlohmann_json
    GITHUB_REPOSITORY nlohmann/json
    VERSION 3.12.0
    SOURCE_DIR ${LIB_DIR}/nlohmann_json
)


#-------------------------------------
#       Project Files
#-------------------------------------

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/)
set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/)
set(EXAMPLE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Example/)


set(Source_Files 
    ${SOURCE_DIR}/LoggerWrapper.cpp
    ${SOURCE_DIR}/Formatter.cpp
    ${SOURCE_DIR}/LoggerImpl.cpp
)


set(Header_Files 
    ${HEADER_DIR}/Logger/Logger.h  
    ${HEADER_DIR}/Logger/LoggerWrapper.h  
    ${HEADER_DIR}/Logger/PrintMacros.h
    ${HEADER_DIR}/Logger/Formatter.h
    ${HEADER_DIR}/Logger/Helper.h
    ${HEADER_DIR}/Logger/LoggerConfig.h
)


set(Example_Files 
    ${EXAMPLE_DIR}/main.cpp
)


#-------------------------------------
#       Create Static Library
#-------------------------------------

add_library(${PROJECT_NAME} STATIC ${Source_Files} ${Header_Files})


target_link_libraries(${PROJECT_NAME} 
PUBLIC  fmt::fmt     
        spdlog::spdlog 
        nlohmann_json::nlohmann_json
)


target_include_directories(${PROJECT_NAME} 
        PUBLIC  
            ${HEADER_DIR}/Logger
            ${LIB_DIR}/fmt/include 
        PRIVATE 
            ${HEADER_DIR}/Logger/detail
)



#-------------------------------------
#       Defining JSON sink config names
#-------------------------------------

set(LOGGER_CONFIG_SINK "sinks" CACHE STRING "JSON key for logger sinks")
set(LOGGER_CONFIG_SINK_TYPE "type" CACHE STRING "JSON key for sink type")
set(LOGGER_CONFIG_LEVEL "level" CACHE STRING "JSON key for log level")
set(LOGGER_CONFIG_MAX_SKIP_DURATION "max_skip_duration" CACHE STRING "JSON key for max skip duration")
set(LOGGER_CONFIG_FILE_NAME "file_name" CACHE STRING "JSON key for file name")
set(LOGGER_CONFIG_MAX_FILE_SIZE "max_file_size" CACHE STRING "JSON key for max file size")
set(LOGGER_CONFIG_MAX_FILES "max_files" CACHE STRING "JSON key for max files")
set(LOGGER_CONFIG_ROTATE_ON_SESSION "rotate_on_session" CACHE STRING "JSON key for file rotation on session")
set(LOGGER_CONFIG_CHECK_FOR_DEBUGGER "check_for_debugger" CACHE STRING "JSON key for MSVC sink debugger check")
set(LOGGER_CONFIG_PATTERN "pattern" CACHE STRING "JSON key for log pattern")


configure_file(LoggerJSONConfigNames.h.in LoggerJSONConfigNames.h @ONLY)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
