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


	CHESS_API void SetDelegate(PFN_CALLBACK pDelegate);


	CHESS_API int  GetNumPossibleMoves(PositionInstance positionInstance);

	CHESS_API bool GetPossibleMoveAtIndex(int index, PositionInstance positionInstance, PossibleMoveInstance *possibleMoveInstance);

	CHESS_API void ExecuteMove(const PossibleMoveInstance& moveInstance);


	//CHESS_API bool GetChessBoard(ChessBoard &board);

	CHESS_API PieceTypeInstance GetPieceInPosition(PositionInstance posInstance);


	CHESS_API void StartGame();

	CHESS_API void EndGame();

	CHESS_API void ResetGame();


#ifdef __cplusplus
}
#endif // __cplusplus
