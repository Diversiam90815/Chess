/*
  ==============================================================================

	Class:          ChessLogicAPI

	Description:    Plain C API for the Chess logic

  ==============================================================================
*/

#include "ChessLogicAPI.h"
#include "GameManager.h"



//=============================================
//			API Helper Functions
//=============================================


PossibleMove MapPossibleMove(const PossibleMoveInstance &moveInstance)
{
	PossibleMove move;

	move.start.x = moveInstance.start.x;
	move.start.y = moveInstance.start.y;
	move.end.x	 = moveInstance.end.x;
	move.end.y	 = moveInstance.end.y;
	move.type	 = static_cast<MoveType>(moveInstance.type);

	return move;
}


Position MapPosition(const PositionInstance positionInstance)
{
	Position pos;

	pos.x = positionInstance.x;
	pos.y = positionInstance.y;

	return pos;
}



//=============================================
//			CHESS API
//=============================================

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
	GameManager *manager = GameManager::GetInstance();
	manager->setDelegate(pDelegate);
}


CHESS_API int GetNumPossibleMoves(PositionInstance pos)
{
	return 0;
}


CHESS_API bool GetPossibleMoveAtIndex(int index, PossibleMoveInstance possibleMove)
{
	return false;
}


CHESS_API void ExecuteMove(const PossibleMoveInstance &moveInstance)
{
	GameManager *manager = GameManager::GetInstance();
	PossibleMove move	 = MapPossibleMove(moveInstance);
	manager->executeMove(move);
}


CHESS_API void StartGame()
{
	GameManager *manager = GameManager::GetInstance();
	manager->init();
}


CHESS_API void EndGame()
{
	GameManager *manager = GameManager::GetInstance();
	manager->endGame();
}


CHESS_API void ResetGame()
{
	GameManager *manager = GameManager::GetInstance();
	manager->resetGame();
}


CHESS_API bool GetPieceInPosition(PositionInstance pos, PieceTypeInstance piece)
{
	return false;
}
