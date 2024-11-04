/*
  ==============================================================================

	Class:          ChessLogicAPI

	Description:    Plain C API for the Chess logic

  ==============================================================================
*/

#pragma once

#include "ChessLogicAPIDefines.h"


#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

	
	CHESS_API void Init();

	CHESS_API void Deinit();


#ifdef __cplusplus
}
#endif // __cplusplus
