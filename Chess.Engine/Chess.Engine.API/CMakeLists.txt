#==========================================
#   Chess Engine API
#       by Jens W. Langenberg
#==========================================

cmake_minimum_required(VERSION 3.25)

set(CHESS_VERSION 1.1.0.745)

project(Chess.Engine.API VERSION ${CHESS_VERSION} LANGUAGES CXX)


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


#==========================================
#   INCLUDE DIRECTORIES
#==========================================

set(ALL_PROJECT_DIRS ${SOURCE_DIR})

include_directories(${ALL_PROJECT_DIRS})


#==========================================
#   SET THE FILES
#==========================================

set(API_FILES
	${SOURCE_DIR}/EngineAPI.h    ${SOURCE_DIR}/EngineAPI.cpp
	${SOURCE_DIR}/EngineAPIDefines.h
)


#==========================================
#   SOURCE GROUPS
#==========================================

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${ALL_FILES})


#==========================================
#   CREATE TARGET
#==========================================


add_library(${PROJECT_NAME} SHARED  ${API_FILES})


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
)

target_include_directories(${PROJECT_NAME} PUBLIC ${Include_Dirs})


#==========================================
#   LINK LIBRARIES
#==========================================

set(PROJECT_LIBS Chess.Engine.Core)

target_link_libraries(${PROJECT_NAME} PUBLIC ${PROJECT_LIBS})

