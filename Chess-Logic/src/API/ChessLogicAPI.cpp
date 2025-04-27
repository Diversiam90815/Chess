/*
  ==============================================================================
	Module:         ChessLogicAPI
	Description:    Plain C API for the Chess logic
  ==============================================================================
*/

#include <strsafe.h>
#include <combaseapi.h>

#include "ChessLogicAPI.h"
#include "GameManager.h"
#include "FileManager.h"
#include "StateMachine.h"



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

	if (index < 0 || index >= static_cast<int>(moves.size()))
		return false;

	// auto it = moves.begin();
	// std::advance(it, index); // Get the iterator to the desired element

	PossibleMove tmpMove = moves.at(index);

	if (tmpMove.isEmpty())
		return false;

	PositionInstance startPos	= MapToPositionInstance(tmpMove.start);
	PositionInstance endPos		= MapToPositionInstance(tmpMove.end);

	possibleMoveInstance->start = startPos;
	possibleMoveInstance->end	= endPos;
	possibleMoveInstance->type	= static_cast<MoveTypeInstance>(tmpMove.type);
	return true;
}


CHESS_API void HandleMoveStateChanged(const PossibleMoveInstance &moveInstance)
{
	// GameManager *manager = GameManager::GetInstance();
	// PossibleMove move	 = MapToPossibleMove(moveInstance);
	// manager->handleMoveStateChanges(move);
}


CHESS_API void ChangeMoveState(int moveState)
{
	// GameManager *manager = GameManager::GetInstance();
	// MoveState	 state	 = (MoveState)moveState;
	// manager->setCurrentMoveState(state);
}


CHESS_API void StartGame()
{
	StateMachine::GetInstance()->onGameStarted();
}


CHESS_API void ResetGame()
{
	StateMachine::GetInstance()->resetGame();
}


CHESS_API void UndoMove()
{
	GameManager::GetInstance()->undoMove();
}


CHESS_API int GetEndgameState()
{
	// TODO
	return 0;
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


CHESS_API void StartMultiplayerGame(bool isHost)
{
	StateMachine::GetInstance()->onMultiplayerGameStarted(isHost);
}


CHESS_API void StartRemoteDiscovery(bool isHost)
{
	GameManager::GetInstance()->startRemoteDiscovery();
}


CHESS_API void DisconnectMultiplayerGame()
{
	return GameManager::GetInstance()->disconnectMultiplayerGame();
}


CHESS_API bool IsMultiplayerActive()
{
	return GameManager::GetInstance()->isMultiplayerActive();
}


CHESS_API void OnSquareSelected(PositionInstance positionInstance)
{
	Position pos = MapToPosition(positionInstance);
	StateMachine::GetInstance()->onSquareSelected(pos);
}


CHESS_API void OnPawnPromotionChosen(PieceTypeInstance promotionInstance)
{
	PieceType promotion = static_cast<PieceType>(promotionInstance);
	StateMachine::GetInstance()->onPawnPromotionChosen(promotion);
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


CHESS_API void SetLocalPlayerName(const char *name)
{
	return GameManager::GetInstance()->setLocalPlayerName(name);
}


CHESS_API char *GetRemotePlayerName()
{
	std::string remoteName = GameManager::GetInstance()->getRemotePlayerName();
	char	   *name	   = StringToCharPtr(remoteName);
	return name;
}


CHESS_API int GetNetworkAdapterCount()
{
	GameManager *manager	 = GameManager::GetInstance();
	auto		 adapters	 = manager->getNetworkAdapters();
	size_t		 numAdapters = adapters.size();
	return numAdapters;
}


CHESS_API bool GetNetworkAdapterAtIndex(unsigned int index, NetworkAdapterInstance *adapter)
{
	GameManager *manager  = GameManager::GetInstance();
	auto		 adapters = manager->getNetworkAdapters();

	std::string	 name{};
	int			 ID{0};
	bool		 selectedByUser{0};
	int			 counter{-1};

	for (auto &adapter : adapters)
	{
		counter++;
		if (counter == index)
		{
			name		   = adapter.description;
			ID			   = adapter.ID;
			selectedByUser = adapter.selected;
			break;
		}
	}

	if (name.empty() && ID == 0)
		return false;

	adapter->ID				= ID;
	adapter->selectedByUser = selectedByUser;
	StringCbCopyA(adapter->name, MAX_STRING_LENGTH, name.c_str());
	return true;
}


CHESS_API int GetSavedAdapterID()
{
	return GameManager::GetInstance()->getCurrentNetworkAdapterID();
}


CHESS_API bool ChangeCurrentAdapter(int ID)
{
	GameManager *manager = GameManager::GetInstance();
	bool		 result	 = manager->changeCurrentNetworkAdapter(ID);
	return result;
}
