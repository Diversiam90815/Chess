/*
  ==============================================================================
	Module:         ChessLogicAPI
	Description:    Plain C API for the Chess logic
  ==============================================================================
*/

#include <strsafe.h>
#include <combaseapi.h>

#include "EngineAPI.h"

#include "GameManager.h"
#include "FileManager.h"
#include "StateMachine.h"
#include "Logging.h"


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

Engine_API void Init()
{
	GameManager *manager = GameManager::GetInstance();
	manager->init();
}


Engine_API void Deinit()
{
	GameManager::ReleaseInstance();
	FileManager::ReleaseInstance();
}


Engine_API void SetDelegate(PFN_CALLBACK pDelegate)
{
	GameManager *manager = GameManager::GetInstance();
	manager->setDelegate(pDelegate);
}


Engine_API float GetWindowScalingFactor(HWND hwnd)
{
	int	  dpi			= GetDpiForWindow(hwnd);
	float scalingFactor = (float)dpi / 96;
	return scalingFactor;
}


Engine_API void SetUnvirtualizedAppDataPath(const char *appDataPath)
{
	FileManager *fmg = FileManager::GetInstance();
	fmg->setAppDataPath(appDataPath);
}


Engine_API int GetNumPossibleMoves()
{
	GameManager *manager  = GameManager::GetInstance();

	auto		 moves	  = manager->getPossibleMoveForPosition();
	size_t		 numMoves = moves.size();
	return numMoves;
}


// Needs to be called when delegate message received that calculation of possible moves is done (GameManager.cpp l211)
Engine_API bool GetPossibleMoveAtIndex(int index, PossibleMoveInstance *possibleMoveInstance)
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


Engine_API void StartGame()
{
	StateMachine::GetInstance()->onGameStarted();
}


Engine_API void ResetGame()
{
	StateMachine::GetInstance()->resetGame();
}


Engine_API void UndoMove()
{
	StateMachine::GetInstance()->reactToUndoMove();
}


Engine_API int GetEndgameState()
{
	// TODO
	return 0;
}


Engine_API bool GetBoardState(int *boardState)
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


Engine_API void StartedMultiplayer()
{
	GameManager::GetInstance()->startedMultiplayer();
}


Engine_API void StartMultiplayerGame(bool isHost)
{
	StateMachine::GetInstance()->onMultiplayerGameStarted(isHost);
}


Engine_API void StartRemoteDiscovery(bool isHost)
{
	GameManager::GetInstance()->startRemoteDiscovery(isHost);
}


Engine_API void DisconnectMultiplayerGame()
{
	return GameManager::GetInstance()->disconnectMultiplayerGame();
}


Engine_API bool IsMultiplayerActive()
{
	return GameManager::GetInstance()->isMultiplayerActive();
}


Engine_API void OnSquareSelected(PositionInstance positionInstance)
{
	Position pos = MapToPosition(positionInstance);
	StateMachine::GetInstance()->onSquareSelected(pos);
}


Engine_API void OnPawnPromotionChosen(PieceTypeInstance promotionInstance)
{
	PieceType promotion = static_cast<PieceType>(promotionInstance);
	StateMachine::GetInstance()->onPawnPromotionChosen(promotion);
}


Engine_API void LogInfoWithCaller(const char *message, const char *method, const char *className, const int lineNumber)
{
	logging::log(LogLevel::Info,className,lineNumber,method, message);
}


Engine_API void LogErrorWithCaller(const char *message, const char *method, const char *className, const int lineNumber)
{
	logging::log(LogLevel::Error, className, lineNumber, method, message);
}


Engine_API void LogWarningWithCaller(const char *message, const char *method, const char *className, const int lineNumber)
{
	logging::log(LogLevel::Warn, className, lineNumber, method, message);
}


Engine_API void SetCurrentBoardTheme(const char *theme)
{
	GameManager *manager = GameManager::GetInstance();
	manager->setBoardTheme(theme);
}


Engine_API char *GetCurrentBoardTheme()
{
	GameManager *manager  = GameManager::GetInstance();
	std::string	 theme	  = manager->getBoardTheme();
	char		*themeTmp = StringToCharPtr(theme);
	return themeTmp;
}


Engine_API void SetCurrentPieceTheme(const char *theme)
{
	GameManager *manager = GameManager::GetInstance();
	manager->setPieceTheme(theme);
}


Engine_API char *GetCurrentPieceTheme()
{
	GameManager *manager  = GameManager::GetInstance();
	std::string	 theme	  = manager->getPieceTheme();
	char		*themeTmp = StringToCharPtr(theme);
	return themeTmp;
}


Engine_API void SetLocalPlayerName(const char *name)
{
	return GameManager::GetInstance()->setLocalPlayerName(name);
}


Engine_API void ApproveConnectionRequest()
{
	GameManager::GetInstance()->approveConnectionRequest();
}


Engine_API void RejectConnectionRequest()
{
	GameManager::GetInstance()->rejectConnectionRequest();
}


Engine_API void SendConnectionRequestToHost()
{
	GameManager::GetInstance()->sendConnectionRequestToHost();
}


Engine_API void StoppedMultiplayer()
{
	GameManager::GetInstance()->stoppedMultiplayer();
}


Engine_API int GetNetworkAdapterCount()
{
	GameManager *manager	 = GameManager::GetInstance();
	auto		 adapters	 = manager->getNetworkAdapters();
	size_t		 numAdapters = adapters.size();
	return numAdapters;
}


Engine_API bool GetNetworkAdapterAtIndex(unsigned int index, NetworkAdapterInstance *adapter)
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


Engine_API int GetSavedAdapterID()
{
	return GameManager::GetInstance()->getCurrentNetworkAdapterID();
}


Engine_API bool ChangeCurrentAdapter(int ID)
{
	GameManager *manager = GameManager::GetInstance();
	bool		 result	 = manager->changeCurrentNetworkAdapter(ID);
	return result;
}
