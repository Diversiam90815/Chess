/*
  ==============================================================================
	Module:         ChessLogicAPI
	Description:    Plain C API for the Chess logic
  ==============================================================================
*/

#include <strsafe.h>

#include "ChessLogicAPI.h"
#include "GameManager.h"
#include "FileManager.h"



//=============================================
//			API Helper Functions
//=============================================


static PossibleMove MapToPossibleMove(const PossibleMoveInstance &moveInstance)
{
	PossibleMove move;

	move.start.x		= moveInstance.start.x;
	move.start.y		= moveInstance.start.y;
	move.end.x			= moveInstance.end.x;
	move.end.y			= moveInstance.end.y;
	move.type			= static_cast<MoveType>(moveInstance.type);
	move.promotionPiece = static_cast<PieceType>(moveInstance.promotionPiece);

	return move;
}


static Position MapToPosition(const PositionInstance positionInstance)
{
	Position pos;

	pos.x = positionInstance.x;
	pos.y = positionInstance.y;

	return pos;
}


static PositionInstance MapToPositionInstance(Position position)
{
	PositionInstance pos;
	pos.x = position.x;
	pos.y = position.y;
	return pos;
}


static char *StringToCharPtr(std::string string)
{
	size_t len	   = string.length() + 1;

	char  *charPtr = (char *)CoTaskMemAlloc(len);

	if (charPtr == nullptr)
		return nullptr;


	HRESULT hr = StringCbCopy(charPtr, len, string.c_str());

	if (FAILED(hr))
	{
		CoTaskMemFree(charPtr);
		return nullptr;
	}

	return charPtr;
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

	Position	 start{-1, -1};
	Position	 end{-1, -1};
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

	if ((start != Position{-1, -1}) && (end != Position{-1, -1}) && (type != MoveType::None))
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
	manager->startGame();
}


CHESS_API void ResetGame()
{
	GameManager *manager = GameManager::GetInstance();
	manager->resetGame();
}


CHESS_API void UndoMove()
{
	GameManager::GetInstance()->undoMove();
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

	LoggingHelper::logBoardState(boardState);
	return true;
}


CHESS_API void LogInfoWithCaller(const char *message, const char *method, const char *className, const int lineNumber)
{
	spdlog::source_loc loc(className, lineNumber, method);
	logging::log(LogLevel::Info, loc, message);
}


CHESS_API void LogErrorWithCaller(const char *message, const char *method, const char *className, const int lineNumber)
{
	spdlog::source_loc loc(className, lineNumber, method);
	logging::log(LogLevel::Error, loc, message);
}


CHESS_API void LogWarningWithCaller(const char *message, const char *method, const char *className, const int lineNumber)
{
	spdlog::source_loc loc(className, lineNumber, method);
	logging::log(LogLevel::Warn, loc, message);
}


CHESS_API void SetCurrentBoardTheme(const char *theme)
{
	GameManager *manager = GameManager::GetInstance();
	manager->setBoardTheme(theme);
}


CHESS_API char *GetCurrentBoardTheme()
{
	GameManager *manager  = GameManager::GetInstance();
	std::string	 theme	  = manager->getBoardTheme();
	char		*themeTmp = StringToCharPtr(theme);
	return themeTmp;
}


CHESS_API void SetCurrentPieceTheme(const char *theme)
{
	GameManager *manager = GameManager::GetInstance();
	manager->setPieceTheme(theme);
}


CHESS_API char *GetCurrentPieceTheme()
{
	GameManager *manager  = GameManager::GetInstance();
	std::string	 theme	  = manager->getPieceTheme();
	char		*themeTmp = StringToCharPtr(theme);
	return themeTmp;
}
