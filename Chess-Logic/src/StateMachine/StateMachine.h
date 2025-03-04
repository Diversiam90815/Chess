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


enum class GameState
{
	Undefined		 = 0,
	InitLocal		 = 1,
	InitLAN			 = 2,
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

	void start();
	void stop();

private:
	StateMachine();

	void					run();

	boost::thread			worker;
	std::atomic<bool>		mRunning;
	std::mutex				mMutex;
	std::condition_variable cv;

	GameState				mCurrentState{GameState::Undefined};
	Position				mMoveStart{};
	Position				mMoveEnd{};
	bool					mMoveStartSet{false};

	bool					mAwaitingPromotion{false};
	PieceType				mPromotionChoice{PieceType::DefaultType};
};
