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

	//void						setCurrentMoveState(MoveState state);
	//MoveState					getCurrentMoveState() const;

	void						resetGame();

	void						endGame(EndGameState state, PlayerColor player) override;

	std::optional<PlayerColor>	getWinner() const;

	void						clearState();

	void						setDelegate(PFN_CALLBACK pDelegate);

	PieceType					getCurrentPieceTypeAtPosition(const Position position);

	std::vector<PossibleMove>	getPossibleMoveForPosition();

	bool						getBoardState(int boardState[BOARD_SIZE][BOARD_SIZE]);

	void						handleMoveStateChanges(PossibleMove &move);

	bool						checkForValidMoves(const PossibleMove &move);
	bool						checkForPawnPromotionMove(const PossibleMove &move);

	void						moveStateInitiated() override; // Let the UI know moves for current round are ready -> handling need to be refactored later!

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

	void						attachObserver(IGameObserver *observer) override;
	void						detachObserver(IGameObserver *observer) override;

	void						switchTurns();

	bool						calculateAllMovesForPlayer();

	bool						initiateMove(const Position &startPosition);

	EndGameState				checkForEndGameConditions();

private:
	GameManager();


	void							 initObservers();
	void							 deinitObservers();


	Logging							 mLog;

	bool							 mMovesGeneratedForCurrentTurn = false;

	Player							 mWhitePlayer;
	Player							 mBlackPlayer;

	PlayerColor						 mCurrentPlayer	   = PlayerColor::NoColor;

	//MoveState						 mCurrentMoveState = MoveState::NoMove;

	std::vector<PossibleMove>		 mAllMovesForPosition;

	std::shared_ptr<ChessBoard>		 mChessBoard;

	std::shared_ptr<MoveGeneration>	 mMoveGeneration;
	std::shared_ptr<MoveValidation>	 mMoveValidation;
	std::shared_ptr<MoveExecution>	 mMoveExecution;

	std::unique_ptr<UICommunication> mUiCommunicationLayer;

	UserSettings					 mUserSettings;

	std::unique_ptr<NetworkManager>	 mNetwork;

	std::vector<IGameObserver *>	 mObservers;
};
