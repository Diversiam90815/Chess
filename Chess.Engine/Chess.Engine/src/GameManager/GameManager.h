/*
  ==============================================================================
	Module:         GameManager
	Description:    Manager for the Chess game
  ==============================================================================
*/

#pragma once

#include "EngineAPIDefines.h"
#include "GameEngine.h"
#include "Player.h"
#include "Logging.h"
#include "UserSettings.h"
#include "IObservable.h"
#include "UICommunication.h"
#include "NetworkManager.h"
#include "MultiplayerManager.h"
#include "PlayerName.h"


class StateMachine;


class GameManager
{
public:
	~GameManager();

	static GameManager		   *GetInstance();
	static void					ReleaseInstance();

	bool						init();

	bool						startGame();

	void						executeMove(PossibleMove &tmpMove, bool fromRemote = false);

	void						undoMove();

	void						resetGame();

	std::optional<PlayerColor>	getWinner() const;

	void						setDelegate(PFN_CALLBACK pDelegate);

	std::vector<PossibleMove>	getPossibleMoveForPosition();

	bool						getBoardState(int boardState[BOARD_SIZE][BOARD_SIZE]);

	bool						checkForValidMoves(const PossibleMove &move);
	bool						checkForPawnPromotionMove(const PossibleMove &move);

	std::vector<NetworkAdapter> getNetworkAdapters();
	bool						changeCurrentNetworkAdapter(int ID);
	int							getCurrentNetworkAdapterID();

	void						setLocalPlayerName(std::string name);
	std::string					getLocalPlayerName();

	void						setBoardTheme(std::string theme) { mUserSettings.setCurrentBoardTheme(theme); }
	std::string					getBoardTheme() { return mUserSettings.getCurrentBoardTheme(); }

	void						setPieceTheme(std::string theme) { mUserSettings.setCurrentPieceTheme(theme); }
	std::string					getPieceTheme() { return mUserSettings.getCurrentPieceTheme(); }

	void						switchTurns();

	bool						calculateAllMovesForPlayer();

	bool						initiateMove(const Position &startPosition);

	EndGameState				checkForEndGameConditions();

	bool						startMultiplayerGame();

	void						disconnectMultiplayerGame();

	void						startedMultiplayer();
	void						stoppedMultiplayer();

	bool						isMultiplayerActive() const;

	bool						isLocalPlayerTurn();

	void						startRemoteDiscovery(bool isHost);

	void						answerConnectionInvitation(bool accepted);
	void						sendConnectionRequestToHost();

	void						setLocalPlayerInMultiplayer(PlayerColor localPlayer);
	void						setLocalPlayerReady(const bool flag);

	void						setSFXEnabled(const bool enabled);
	bool						getSFXEnabled();

	void						setAtmosEnabled(const bool enabled);
	bool						getAtmosEnabled();

	void						setSFXVolume(const float volume);
	float						getSFXVolume();

	void						setAtmosVolume(const float volume);
	float						getAtmosVolume();

	void						setMasterAudioVolume(const float volume);
	float						getMasterVolume();

	void						setAtmosScenario(const std::string scenario);
	std::string					getAtmosScenario();


private:
	GameManager() = default;

	void								initObservers();
	void								initMultiplayerObservers();
	void								deinitObservers();


	Logging								mLog;

	UserSettings						mUserSettings;

	PlayerName							mPlayerName;

	std::shared_ptr<GameEngine>			mEngine;

	std::shared_ptr<UICommunication>	mUiCommunicationLayer;

	std::shared_ptr<MultiplayerManager> mMultiplayerManager;
	std::unique_ptr<NetworkManager>		mNetwork;

	bool								mIsMultiplayerMode{false};
};
