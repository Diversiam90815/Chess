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


PossibleMove MapToPossibleMove(const PossibleMoveInstance &moveInstance)
{
	PossibleMove move;

	move.start.x = moveInstance.start.x;
	move.start.y = moveInstance.start.y;
	move.end.x	 = moveInstance.end.x;
	move.end.y	 = moveInstance.end.y;
	move.type	 = static_cast<MoveType>(moveInstance.type);

	return move;
}


Position MapToPosition(const PositionInstance positionInstance)
{
	Position pos;

	pos.x = positionInstance.x;
	pos.y = positionInstance.y;

	return pos;
}


PositionInstance MapToPositionInstance(Position position)
{
	PositionInstance pos;
	pos.x = position.x;
	pos.y = position.y;
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


CHESS_API int GetNumPossibleMoves(PositionInstance positionInstance)
{
	GameManager *manager  = GameManager::GetInstance();
	Position	 pos	  = MapToPosition(positionInstance);

	auto		 moves	  = manager->mMovementManager->getMovesForPosition(pos);
	size_t		 numMoves = moves.size();
	return numMoves;
}


CHESS_API bool GetPossibleMoveAtIndex(int index, PositionInstance positionInstance, PossibleMoveInstance *possibleMoveInstance)
{
	GameManager *manager = GameManager::GetInstance();
	Position	 pos	 = MapToPosition(positionInstance);

	auto		 moves	 = manager->mMovementManager->getMovesForPosition(pos);

	Position	 start{0, 0};
	Position	 end{0, 0};
	MoveType	 type	 = MoveType::None;

	int			 counter = -1;

	for (auto &move : moves)
	{
		counter++;
		if (counter == index)
		{
			start = move.start;
			end	  = move.end;
			type  = move.type;
			break;
		}
	}

	if ((start != Position{0, 0}) && (end != Position{0, 0}) && ((type & MoveType::None) != MoveType::None))
	{
		PositionInstance startPos	= MapToPositionInstance(start);
		PositionInstance endPos		= MapToPositionInstance(end);

		possibleMoveInstance->start = startPos;
		possibleMoveInstance->end	= endPos;
		possibleMoveInstance->type	= static_cast<MoveTypeInstance>(type);
		return true;
	}
	return false;
}


CHESS_API void ExecuteMove(const PossibleMoveInstance &moveInstance)
{
	GameManager *manager = GameManager::GetInstance();
	PossibleMove move	 = MapToPossibleMove(moveInstance);
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


CHESS_API PieceTypeInstance GetPieceInPosition(PositionInstance posInstance)
{
	Position pos = MapToPosition(posInstance);

	if (pos.x < 0 || pos.x >= BOARD_SIZE || pos.y < 0 || pos.y >= BOARD_SIZE)
	{
		return PieceTypeInstance::DefaultType;
	}

	GameManager *manager = GameManager::GetInstance();

	PieceType	 type	 = manager->getCurrentPieceTypeAtPosition(pos);
	return static_cast<PieceTypeInstance>(type);
}
