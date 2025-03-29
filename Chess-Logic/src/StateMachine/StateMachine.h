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
// #include "GameManager.h"
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
	void	  onSquareSelected(const Position &pos);	  // Called from UI
	void	  onPawnPromotionChosen(PieceType promotion); // Called from UI

	void	  gameStateChanged(const GameState state) override;
	GameState getGameState() const;

	bool	  isInitialized() const;
	void	  setInitialized(const bool value);

	void	  attachObserver(std::weak_ptr<IGameStateObserver> observer) override;
	void	  detachObserver(std::weak_ptr<IGameStateObserver> observer) override;

	void	  triggerEvent();

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

	GameState									   mCurrentState{GameState::Undefined};
	// Position						  mMoveStart{};
	// Position						  mMoveEnd{};
	PossibleMove								   mCurrentPossibleMove{};

	bool										   mMovesCalulated{false};

	bool										   mWaitingForTargetStart{false};
	bool										   mWaitingForTargetEnd{false};

	bool										   mIsValidMove{false};

	bool										   mAwaitingPromotion{false};
	PieceType									   mPromotionChoice{PieceType::DefaultType};

	EndGameState								   mEndgameState{EndGameState::OnGoing};

	std::vector<std::weak_ptr<IGameStateObserver>> mObservers;
};
