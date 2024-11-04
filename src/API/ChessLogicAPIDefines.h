/*
  ==============================================================================

	Class:          ChessLogicAPIDefines

	Description:    Definitions for the Chess Logic API

  ==============================================================================
*/

#pragma once
#include "Windows.h"


// Delegate callback

typedef void(CALLBACK *PFN_CALLBACK)(int messageId, void*pContext);

#define CHESS_API __declspec(dllexport)
#define MAX_STRING_LENGTH 250

