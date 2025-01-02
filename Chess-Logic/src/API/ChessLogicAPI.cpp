/*
  ==============================================================================

	Class:          ChessLogicAPI

	Description:    Plain C API for the Chess logic

  ==============================================================================
*/

#include "ChessLogicAPI.h"
#include "GameManager.h"
#include "FileManager.h"



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

	FileManager::ReleaseInstance();
}


CHESS_API void SetDelegate(PFN_CALLBACK pDelegate)
{
	GameManager *manager = GameManager::GetInstance();
	manager->setDelegate(pDelegate);
}


CHESS_API float GetWindowScalingFactor(HWND hwnd)
{
	int	  dpi			= GetDpiForWindow(hwnd);
	float scalingFactor = (float)dpi / 96;
	return scalingFactor;
}


CHESS_API void SetUnvirtualizedAppDataPath(const char *appDataPath)
{
	FileManager *fmg = FileManager::GetInstance();
	fmg->setAppDataPath(appDataPath);
}


CHESS_API int GetNumPossibleMoves()
{
	GameManager *manager  = GameManager::GetInstance();

	auto		 moves	  = manager->getPossibleMoveForPosition();
	size_t		 numMoves = moves.size();
	return numMoves;
}


// Needs to be called when delegate message received that calculation of possible moves is done (GameManager.cpp l211)
CHESS_API bool GetPossibleMoveAtIndex(int index, PossibleMoveInstance *possibleMoveInstance)
{
	GameManager *manager = GameManager::GetInstance();

	auto		 moves	 = manager->getPossibleMoveForPosition();

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

	if ((start != Position{0, 0}) && (end != Position{0, 0}) && (type != MoveType::None))
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


CHESS_API void HandleMoveStateChanged(const PossibleMoveInstance &moveInstance)
{
	GameManager *manager = GameManager::GetInstance();
	PossibleMove move	 = MapToPossibleMove(moveInstance);
	manager->handleMoveStateChanges(move);
}


CHESS_API void ChangeMoveState(int moveState)
{
	GameManager *manager = GameManager::GetInstance();
	MoveState	 state	 = (MoveState)moveState;
	manager->setCurrentMoveState(state);
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


CHESS_API void GetPieceInPosition(PositionInstance posInstance, PieceTypeInstance *pieceTypeInstance)
{
	Position		  pos	= MapToPosition(posInstance);
	PieceTypeInstance piece = PieceTypeInstance::DefaultType;

	if (pos.x < 0 || pos.x >= BOARD_SIZE || pos.y < 0 || pos.y >= BOARD_SIZE)
	{
		*pieceTypeInstance = piece;
		return;
	}

	GameManager *manager = GameManager::GetInstance();

	PieceType	 type	 = manager->getCurrentPieceTypeAtPosition(pos);
	*pieceTypeInstance	 = static_cast<PieceTypeInstance>(static_cast<int>(type));
}


CHESS_API bool GetBoardState(int *boardState)
{
	if (!boardState)
		return false;

	GameManager *manager = GameManager::GetInstance();
	if (!manager)
		return false;

	int	 localBoardState[BOARD_SIZE][BOARD_SIZE];

	bool result = manager->getBoardState(localBoardState);

	if (!result)
		return false;

	for (int y = 0; y < BOARD_SIZE; ++y)
	{
		for (int x = 0; x < BOARD_SIZE; ++x)
		{
			// Mapping 2D array to 1D
			boardState[y * BOARD_SIZE + x] = static_cast<int>(localBoardState[y][x]);
		}
	}

	return true;
}
