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
#include "ThreadBase.h"

class GameManager;


class StateMachine : public IGameStateObservable, public ThreadBase, public IRemoteMessagesObserver, public std::enable_shared_from_this<StateMachine>
{
public:
	static std::shared_ptr<StateMachine> GetInstance();
	static void							 ReleaseInstance();

	~StateMachine();

	void	  onGameStarted();							  // Called from UI
	void	  onMultiplayerGameStarted(bool isHost);	  // Called from UI
	void	  onSquareSelected(const Position &pos);	  // Called from UI
	void	  onPawnPromotionChosen(PieceType promotion); // Called from UI

	void	  gameStateChanged(const GameState state) override;
	GameState getCurrentGameState() { return mCurrentState.load(); }
	void	  setCurrrentGameState(const GameState state) { mCurrentState.store(state); }

	void	  onRemoteMoveReceived(const PossibleMove &remoteMove) override;
	void	  onRemoteChatMessageReceived(const std::string &mesage) override {}

	bool	  isInitialized() const;
	void	  setInitialized(const bool value);

	void	  resetGame();

private:
	StateMachine();

	void				   run() override;

	bool				   handleInitState(bool multiplayer) const;
	bool				   handleWaitingForInputState();
	bool				   handleMoveInitiatedState() const;
	bool				   handleWaitingForTargetState();
	bool				   handleValidatingMoveState();
	bool				   handleExecutingMoveState();
	bool				   handlePawnPromotionState();
	bool				   handleGameOverState();

	void				   switchToNextState();

	bool				   isGameOngoing() const { return mEndgameState == EndGameState::OnGoing; }

	void				   resetCurrentPossibleMove();

	std::atomic<bool>	   mInitialized{false};

	std::atomic<GameState> mCurrentState{GameState::Undefined};

	PossibleMove		   mCurrentPossibleMove{};

	bool				   mMovesCalulated{false};

	bool				   mWaitingForTargetStart{false};
	bool				   mWaitingForTargetEnd{false};

	bool				   mIsValidMove{false};

	bool				   mAwaitingPromotion{false};

	EndGameState		   mEndgameState{EndGameState::OnGoing};

	bool				   mIsMultiplayerGame{false};
	bool				   mIsLocalHost{false};

	std::mutex			   mStateChangedMutex;
	bool				   mHasPendingStateChange{false};
	GameState			   mPendingState{GameState::Undefined};
};
