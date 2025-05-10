/*
  ==============================================================================
	Module:         GameManager
	Description:    Manager for the Chess game
  ==============================================================================
*/

#pragma once

#include <optional>

#include "ChessLogicAPIDefines.h"
#include "MoveExecution.h"
#include "MoveGeneration.h"
#include "MoveValidation.h"
#include "Player.h"
#include "Logging.h"
#include "UserSettings.h"
#include "IObservable.h"
#include "UICommunication.h"
#include "NetworkManager.h"
#include "MultiplayerManager.h"


class StateMachine;


class GameManager : public IGameObservable
{
public:
	~GameManager();

	static GameManager		   *GetInstance();
	static void					ReleaseInstance();

	bool						init();

	bool						startGame();

	void						executeMove(PossibleMove &tmpMove);

	void						undoMove();

	void						resetGame();

	void						endGame(EndGameState state, PlayerColor player) override;

	std::optional<PlayerColor>	getWinner() const;

	void						clearState();

	void						setDelegate(PFN_CALLBACK pDelegate);

	PieceType					getCurrentPieceTypeAtPosition(const Position position);

	std::vector<PossibleMove>	getPossibleMoveForPosition();

	bool						getBoardState(int boardState[BOARD_SIZE][BOARD_SIZE]);

	bool						checkForValidMoves(const PossibleMove &move);
	bool						checkForPawnPromotionMove(const PossibleMove &move);

	std::vector<NetworkAdapter> getNetworkAdapters();
	bool						changeCurrentNetworkAdapter(int ID);
	int							getCurrentNetworkAdapterID();

	void						setLocalPlayerName(std::string name);

	void						changeCurrentPlayer(PlayerColor player) override;
	PlayerColor					getCurrentPlayer() const;

	void						setBoardTheme(std::string theme) { mUserSettings.setCurrentBoardTheme(theme); }
	std::string					getBoardTheme() const { return mUserSettings.getCurrentBoardTheme(); }

	void						setPieceTheme(std::string theme) { mUserSettings.setCurrentPieceTheme(theme); }
	std::string					getPieceTheme() const { return mUserSettings.getCurrentPieceTheme(); }

	void						switchTurns();

	bool						calculateAllMovesForPlayer();

	bool						initiateMove(const Position &startPosition);

	EndGameState				checkForEndGameConditions();

	bool						startMultiplayerGame(bool isHost);

	void						disconnectMultiplayerGame();

	bool						isMultiplayerActive() const;

	bool						isLocalPlayerTurn();

	void						startRemoteDiscovery(bool isHost);

	void						approveConnectionRequest();
	void						rejectConnectionRequest();
	void						sendConnectionRequestToHost();

private:
	GameManager() = default;

	void								initObservers();
	void								initMultiplayerObservers();
	void								deinitObservers();


	Logging								mLog;

	UserSettings						mUserSettings;

	bool								mMovesGeneratedForCurrentTurn = false;

	Player								mWhitePlayer;
	Player								mBlackPlayer;

	PlayerColor							mCurrentPlayer = PlayerColor::NoColor;

	std::vector<PossibleMove>			mAllMovesForPosition;

	std::shared_ptr<ChessBoard>			mChessBoard;

	std::shared_ptr<MoveGeneration>		mMoveGeneration;
	std::shared_ptr<MoveValidation>		mMoveValidation;
	std::shared_ptr<MoveExecution>		mMoveExecution;

	std::shared_ptr<UICommunication>	mUiCommunicationLayer;

	std::shared_ptr<NetworkManager>		mNetwork;

	std::shared_ptr<MultiplayerManager> mMultiplayerManager;


	bool								mIsMultiplayerMode{false};
	bool								mIsHost{false};
};
