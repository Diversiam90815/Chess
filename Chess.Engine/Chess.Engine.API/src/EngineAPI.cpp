/*
  ==============================================================================
	Module:         Engine API
	Description:    Plain C API for the Chess engine
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
	GameManager::GetInstance()->init();
}


Engine_API void Deinit()
{
	GameManager::ReleaseInstance();
	FileManager::ReleaseInstance();
}


Engine_API void SetDelegate(PFN_CALLBACK pDelegate)
{
	GameManager::GetInstance()->setDelegate(pDelegate);
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
	auto   moves	= GameManager::GetInstance()->getPossibleMoveForPosition();
	size_t numMoves = moves.size();
	return numMoves;
}


// Needs to be called when delegate message received that calculation of possible moves is done (GameManager.cpp l211)
Engine_API bool GetPossibleMoveAtIndex(int index, PossibleMoveInstance *possibleMoveInstance)
{
	GameManager *manager = GameManager::GetInstance();

	auto		 moves	 = manager->getPossibleMoveForPosition();

	if (index < 0 || index >= static_cast<int>(moves.size()))
		return false;

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


Engine_API void StartGame(GameConfiguration config)
{
	StateMachine::GetInstance()->onGameStarted(config);
}


Engine_API void ResetGame()
{
	StateMachine::GetInstance()->resetGame();
}


Engine_API void UndoMove()
{
	StateMachine::GetInstance()->reactToUndoMove();
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


Engine_API void StartMultiplayerGame()
{
	StateMachine::GetInstance()->onMultiplayerGameStarted();
}


Engine_API void StartRemoteDiscovery(bool isHost)
{
	GameManager::GetInstance()->startRemoteDiscovery(isHost);
}


Engine_API void DisconnectMultiplayerGame()
{
	return GameManager::GetInstance()->disconnectMultiplayerGame();
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
	logging::log(LogLevel::Info, className, lineNumber, method, message);
}


Engine_API void LogErrorWithCaller(const char *message, const char *method, const char *className, const int lineNumber)
{
	logging::log(LogLevel::Error, className, lineNumber, method, message);
}


Engine_API void LogWarningWithCaller(const char *message, const char *method, const char *className, const int lineNumber)
{
	logging::log(LogLevel::Warn, className, lineNumber, method, message);
}


Engine_API void LogDebugWithCaller(const char *message, const char *method, const char *className, const int lineNumber)
{
	logging::log(LogLevel::Debug, className, lineNumber, method, message);
}


Engine_API void SetCurrentBoardTheme(const char *theme)
{
	GameManager::GetInstance()->setBoardTheme(theme);
}


Engine_API char *GetCurrentBoardTheme()
{
	std::string theme	 = GameManager::GetInstance()->getBoardTheme();
	char	   *themeTmp = StringToCharPtr(theme);
	return themeTmp;
}


Engine_API void SetCurrentPieceTheme(const char *theme)
{
	GameManager::GetInstance()->setPieceTheme(theme);
}


Engine_API char *GetCurrentPieceTheme()
{
	std::string theme	 = GameManager::GetInstance()->getPieceTheme();
	char	   *themeTmp = StringToCharPtr(theme);
	return themeTmp;
}


Engine_API void SetLocalPlayerName(const char *name)
{
	return GameManager::GetInstance()->setLocalPlayerName(name);
}


Engine_API char *GetLocalPlayerName()
{
	std::string name	= GameManager::GetInstance()->getLocalPlayerName();
	char	   *tmpName = StringToCharPtr(name);
	return tmpName;
}


Engine_API bool GetSFXEnabled()
{
	return GameManager::GetInstance()->getSFXEnabled();
}


Engine_API void SetSFXEnabled(bool enabled)
{
	GameManager::GetInstance()->setSFXEnabled(enabled);
}


Engine_API bool GetAtmosEnabled()
{
	return GameManager::GetInstance()->getAtmosEnabled();
}


Engine_API void SetAtmosEnabled(bool enabled)
{
	GameManager::GetInstance()->setAtmosEnabled(enabled);
}


Engine_API void SetSFXVolume(float volume)
{
	GameManager::GetInstance()->setSFXVolume(volume);
}


Engine_API float GetSFXVolume()
{
	return GameManager::GetInstance()->getSFXVolume();
}


Engine_API void SetAtmosVolume(float volume)
{
	GameManager::GetInstance()->setAtmosVolume(volume);
}


Engine_API float GetAtmosVolume()
{
	return GameManager::GetInstance()->getAtmosVolume();
}


Engine_API void SetAtmosScenario(const char *scenario)
{
	GameManager::GetInstance()->setAtmosScenario(scenario);
}


Engine_API char *GetAtmosScenario()
{
	std::string sScenario = GameManager::GetInstance()->getAtmosScenario();
	char	   *scenario  = StringToCharPtr(sScenario);
	return scenario;
}


Engine_API void SetMasterVolume(float volume)
{
	GameManager::GetInstance()->setMasterAudioVolume(volume);
}


Engine_API float GetMasterVolume()
{
	return GameManager::GetInstance()->getMasterVolume();
}


Engine_API void AnswerConnectionInvitation(bool accept)
{
	GameManager::GetInstance()->answerConnectionInvitation(accept);
}


Engine_API void SendConnectionRequestToHost()
{
	GameManager::GetInstance()->sendConnectionRequestToHost();
}


Engine_API void StoppedMultiplayer()
{
	GameManager::GetInstance()->stoppedMultiplayer();
}


Engine_API void SetLocalPlayer(int iLocalPlayer)
{
	PlayerColor local = (PlayerColor)iLocalPlayer;
	GameManager::GetInstance()->setLocalPlayerInMultiplayer(local);
}


Engine_API void SetLocalPlayerReady(bool ready)
{
	GameManager::GetInstance()->setLocalPlayerReady(ready);
}


Engine_API int GetNetworkAdapterCount()
{
	auto   adapters	   = GameManager::GetInstance()->getNetworkAdapters();
	size_t numAdapters = adapters.size();
	return numAdapters;
}


Engine_API bool GetNetworkAdapterAtIndex(unsigned int index, NetworkAdapterInstance *adapter)
{
	auto adapters = GameManager::GetInstance()->getNetworkAdapters();

	if (index >= adapters.size())
		return false;

	const auto &foundAdapter = adapters[index];

	adapter->ID				 = foundAdapter.ID;
	StringCbCopyA(adapter->name, MAX_STRING_LENGTH, foundAdapter.Description.c_str());
	return true;
}


Engine_API int GetSavedAdapterID()
{
	return GameManager::GetInstance()->getCurrentNetworkAdapterID();
}


Engine_API bool ChangeCurrentAdapter(int ID)
{
	bool result = GameManager::GetInstance()->changeCurrentNetworkAdapter(ID);
	return result;
}
