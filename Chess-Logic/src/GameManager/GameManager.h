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
#include "MessageManager.h"


class StateMachine;


class GameManager : public IGameObservable
{
public:
	~GameManager();

	static GameManager		   *GetInstance();
	static void					ReleaseInstance();

	bool						init();

	void						startGame();

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

	std::string					getRemotePlayerName();
	void						setLocalPlayerName(std::string name);

	void						changeCurrentPlayer(PlayerColor player) override;
	PlayerColor					getCurrentPlayer() const;

	void						setBoardTheme(std::string theme) { mUserSettings.setCurrentBoardTheme(theme); }
	std::string					getBoardTheme() const { return mUserSettings.getCurrentBoardTheme(); }

	void						setPieceTheme(std::string theme) { mUserSettings.setCurrentPieceTheme(theme); }
	std::string					getPieceTheme() const { return mUserSettings.getCurrentPieceTheme(); }

	void						attachObserver(std::weak_ptr<IGameObserver> observer) override;
	void						detachObserver(std::weak_ptr<IGameObserver> observer) override;

	void						switchTurns();

	bool						calculateAllMovesForPlayer();

	bool						initiateMove(const Position &startPosition);

	EndGameState				checkForEndGameConditions();


	bool						startMultiplayerGame(bool isHost);

	bool						connectToRemote(const std::string &remoteIP, const int remotePort);
	void						disconnectMultiplayerGame();

	bool						isMultiplayerActive() const;


private:
	GameManager();

	void									  initObservers();
	void									  deinitObservers();


	Logging									  mLog;

	UserSettings							  mUserSettings;

	bool									  mMovesGeneratedForCurrentTurn = false;

	Player									  mWhitePlayer;
	Player									  mBlackPlayer;

	PlayerColor								  mCurrentPlayer = PlayerColor::NoColor;

	std::vector<PossibleMove>				  mAllMovesForPosition;

	std::shared_ptr<ChessBoard>				  mChessBoard;

	std::shared_ptr<MoveGeneration>			  mMoveGeneration;
	std::shared_ptr<MoveValidation>			  mMoveValidation;
	std::shared_ptr<MoveExecution>			  mMoveExecution;

	std::shared_ptr<UICommunication>		  mUiCommunicationLayer;

	std::shared_ptr<NetworkManager>			  mNetwork;

	std::shared_ptr<MessageManager>			  mMessageManager;


	bool									  mIsMultiplayerMode{false};
	bool									  mIsHost{false};

	std::vector<std::weak_ptr<IGameObserver>> mObservers;
};
