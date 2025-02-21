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


class GameManager : public IGameObservable
{
public:
	~GameManager() = default;

	static GameManager		  *GetInstance();
	static void				   ReleaseInstance();

	void					   init();

	void					   startGame();

	void					   executeMove(PossibleMove &move);

	void					   undoMove();

	void					   gameStateChanged(GameState state) override;
	GameState				   getCurrentGameState() const;

	void					   setCurrentMoveState(MoveState state);
	MoveState				   getCurrentMoveState() const;

	void					   resetGame();

	void					   endGame(PlayerColor player) override;

	std::optional<PlayerColor> getWinner() const;

	void					   clearState();

	void					   setDelegate(PFN_CALLBACK pDelegate);

	PieceType				   getCurrentPieceTypeAtPosition(const Position position);

	std::vector<PossibleMove>  getPossibleMoveForPosition();

	bool					   getBoardState(int boardState[BOARD_SIZE][BOARD_SIZE]);

	void					   handleMoveStateChanges(PossibleMove &move);

	void					   changeCurrentPlayer(PlayerColor player) override;
	PlayerColor				   getCurrentPlayer() const;

	void					   setBoardTheme(std::string theme) { mUserSettings.setCurrentBoardTheme(theme); }
	std::string				   getBoardTheme() const { return mUserSettings.getCurrentBoardTheme(); }

	void					   setPieceTheme(std::string theme) { mUserSettings.setCurrentPieceTheme(theme); }
	std::string				   getPieceTheme() const { return mUserSettings.getCurrentPieceTheme(); }

	void					   attachObserver(IGameObserver *observer) override;
	void					   detachObserver(IGameObserver *observer) override;

private:
	GameManager();

	void							switchTurns();

	void							checkForEndGameConditions();

	Logging							mLog;

	bool							mMovesGeneratedForCurrentTurn = false;

	Player							mWhitePlayer;
	Player							mBlackPlayer;

	PlayerColor						mCurrentPlayer	  = PlayerColor::NoColor;

	GameState						mCurrentState	  = GameState::Init;

	MoveState						mCurrentMoveState = MoveState::NoMove;

	std::vector<PossibleMove>		mAllMovesForPosition;

	std::shared_ptr<ChessBoard>		mChessBoard;

	std::shared_ptr<MoveGeneration> mMoveGeneration;
	std::shared_ptr<MoveValidation> mMoveValidation;
	std::shared_ptr<MoveExecution>	mMoveExecution;

	UserSettings					mUserSettings;

	std::vector<IGameObserver *>	mObservers;

	PFN_CALLBACK					mDelegate = nullptr;
};
