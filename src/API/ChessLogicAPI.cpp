/*
  ==============================================================================

	Class:          ChessLogicAPI

	Description:    Plain C API for the Chess logic

  ==============================================================================
*/

#include "ChessLogicAPI.h"
#include "GameManager.h"


CHESS_API void Init()
{
	GameManager *manager = GameManager::GetInstance();
	manager->init();
}


CHESS_API void Deinit()
{
	GameManager::ReleaseInstance();
}


CHESS_API void SetDelegate(PFN_CALLBACK pDelegate)
{
	GameManager *gameManger = GameManager::GetInstance();
	gameManger->setDelegate(pDelegate);
}


CHESS_API int GetNumPossibleMoves(PositionInstance pos)
{
	return 0;
}


CHESS_API bool GetPossibleMoveAtIndex(int index, PossibleMoveInstance possibleMove)
{
	return false;
}


CHESS_API void ExecuteMove(PossibleMoveInstance &move)
{
}

//
// CHESS_API bool GetChessBoard(ChessBoard &board)
//{
//	return false;
//}


CHESS_API void StartGame()
{
}


CHESS_API void EndGame()
{
}


CHESS_API void ResetGame()
{
}


CHESS_API bool GetPieceInPosition(PositionInstance pos, PieceTypeInstance piece)
{
	return false;
}
