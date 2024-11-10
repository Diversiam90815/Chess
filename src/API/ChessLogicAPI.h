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


	CHESS_API int  GetNumPossibleMoves(PositionInstance pos);

	CHESS_API bool GetPossibleMoveAtIndex(int index, PossibleMoveInstance possibleMove);

	CHESS_API void ExecuteMove(PossibleMoveInstance& move);


	//CHESS_API bool GetChessBoard(ChessBoard &board);

	CHESS_API bool GetPieceInPosition(PositionInstance pos, PieceTypeInstance piece);


	CHESS_API void StartGame();

	CHESS_API void EndGame();

	CHESS_API void ResetGame();


#ifdef __cplusplus
}
#endif // __cplusplus
