/*
  ==============================================================================
	Module:         StateMachine
	Description:    Thread managing the game states
  ==============================================================================
*/

#pragma once

#include <boost/thread.hpp>
#include <mutex>
#include <atomic>
#include <condition_variable>

#include "Parameters.h"
#include "Move.h"
#include "GameManager.h"


enum class GameState
{
	Undefined		 = 0,
	Init			 = 1,
	WaitingForInput	 = 3,
	MoveInitiated	 = 4,
	WaitingForTarget = 5,
	ValidatingMove	 = 6,
	ExecutingMove	 = 7,
	PawnPromotion	 = 8,
	GameOver		 = 9
};


enum class EndGameState
{
	Checkmate = 1,
	StaleMate = 2,
	Reset	  = 3
};


class StateMachine
{
public:
	static StateMachine *GetInstance();
	static void			 ReleaseInstance();

	~StateMachine();

	void	  start();
	void	  stop();

	void	  onGameStarted();							  // Called from UI
	void	  onSquareSelected(const Position &pos);	  // Called from UI
	void	  onPawnPromotionChosen(PieceType promotion); // Called from UI

	void	  setGameState(const GameState state);
	GameState getGameState() const;

	bool	  isInitialized() const;
	void	  setInitialized(const bool value);

private:
	StateMachine();

	void					run();

	bool					handleInitState(bool multiplayer);
	bool					handleWaitingForInputState();
	bool					handleMoveInitiatedState();
	bool					handleWaitingForTargetState();
	bool					handleValidatingMoveState();
	bool					handleExecutingMoveState();
	bool					handlePawnPromotionState();
	bool					handleGameOverState();

	bool					switchToNextState();


	boost::thread			worker;
	std::atomic<bool>		mRunning;
	std::mutex				mMutex;
	std::condition_variable cv;

	std::atomic<bool>		mInitialized{false};

	GameState				mCurrentState{GameState::Undefined};
	Position				mMoveStart{};
	Position				mMoveEnd{};

	bool					mMovesCalulated{false};

	bool					mWaitingForTargetStart{false};
	bool					mWaitingForTargetEnd{false};

	bool					mIsValidMove{false};

	bool					mAwaitingPromotion{false};
	PieceType				mPromotionChoice{PieceType::DefaultType};
};
