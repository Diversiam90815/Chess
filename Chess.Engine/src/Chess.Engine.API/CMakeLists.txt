
set (API_FILES_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/src)

set (API_FILES			${API_FILES_DIR}/EngineAPI.h    
						${API_FILES_DIR}/EngineAPI.cpp)

set (Include_Dirs 		${PROJECT_BINARY_DIR}			
						${API_FILES_DIR})

set (PROJECT_LIBS		Chess.Engine.Core)


include_directories(${API_FILES_DIR})

source_group("API" FILES ${API_FILES})

add_library(Chess.Engine.API SHARED ${API_FILES})

target_include_directories(Chess.Engine.API PUBLIC ${Include_Dirs})

target_link_libraries(Chess.Engine.API PUBLIC ${PROJECT_LIBS})
