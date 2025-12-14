if(ENABLE_FORMAT) 
    include(Format)
endif()

if(ENABLE_CPPCHECK)
    include(CppCheck)
endif()

if(ENABLE_DOXYGEN)
    include(Doxygen)
endif()

add_subdirectory(Chess.Engine.Core)
add_subdirectory(Chess.Engine.API)
add_subdirectory((Chess.Engine.ConsoleApp))
