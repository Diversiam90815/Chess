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
#include "IObservable.h"


class GameManager;


class StateMachine : public IGameStateObservable
{
public:
	static StateMachine *GetInstance();
	static void			 ReleaseInstance();

	~StateMachine();

	void	  start();
	void	  stop();

	void	  onGameStarted();							  // Called from UI
	void	  onMultiplayerGameStarted(bool isHost);
	void	  onSquareSelected(const Position &pos);	  // Called from UI
	void	  onPawnPromotionChosen(PieceType promotion); // Called from UI

	void	  gameStateChanged(const GameState state) override;
	GameState getCurrentGameState() { return mCurrentState.load(); }
	void	  setCurrrentGameState(const GameState state) { mCurrentState.store(state); }

	bool	  isInitialized() const;
	void	  setInitialized(const bool value);

	void	  triggerEvent();

	void	  resetGame();


private:
	StateMachine();

	void										   run();

	bool										   handleInitState(bool multiplayer);
	bool										   handleWaitingForInputState();
	bool										   handleMoveInitiatedState();
	bool										   handleWaitingForTargetState();
	bool										   handleValidatingMoveState();
	bool										   handleExecutingMoveState();
	bool										   handlePawnPromotionState();
	bool										   handleGameOverState();

	void										   switchToNextState();

	bool										   isGameOngoing() const { return mEndgameState == EndGameState::OnGoing; }

	void										   resetCurrentPossibleMove();


	boost::thread								   worker;
	std::atomic<bool>							   mRunning;
	std::mutex									   mMutex;
	std::condition_variable						   cv;

	std::atomic<bool>							   mInitialized{false};
	bool										   mEventTriggered{false};

	std::atomic<GameState>						   mCurrentState{GameState::Undefined};

	PossibleMove								   mCurrentPossibleMove{};

	bool										   mMovesCalulated{false};

	bool										   mWaitingForTargetStart{false};
	bool										   mWaitingForTargetEnd{false};

	bool										   mIsValidMove{false};

	bool										   mAwaitingPromotion{false};
	PieceType									   mPromotionChoice{PieceType::DefaultType};

	EndGameState								   mEndgameState{EndGameState::OnGoing};

	bool										   mIsMultiplayerGame{false};
	bool										   mIsLocalHost{false};
};
