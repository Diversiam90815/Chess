/*
  ==============================================================================
	Module:         StateMachine
	Description:    Thread managing the game states
  ==============================================================================
*/

#pragma once

#include <queue>
#include <mutex>
#include <atomic>
#include <condition_variable>

#include "Logging.h"
#include "Parameters.h"
#include "Move.h"
#include "ThreadBase.h"
#include "IGameController.h"
#include "IInputSource.h"
#include "InputEvent.h"


/**
 * @brief	Pure state machine - only manages transitions.
 *			All game logic delegated to IGameController.
 *			All UI notifications delegated to IInputSource.
 */
class StateMachine : public ThreadBase
{
public:
	StateMachine() = default;
	~StateMachine() override;

	// Non-copyable
	StateMachine(const StateMachine &)						= delete;
	StateMachine		   &operator=(const StateMachine &) = delete;

	//=========================================================================
	// Dependency Injection
	//=========================================================================

	void					setGameController(IGameController *controller);
	void					setInputSource(IInputSource *source);

	//=========================================================================
	// Configuration
	//=========================================================================

	void					setMultiplayerMode(bool enabled) { mIsMultiplayer.store(enabled); }
	void					setVsCPUMode(bool enabled) { mIsVsCPU.store(enabled); }

	//=========================================================================
	// Event Input (thread-safe, can be called from any thread)
	//=========================================================================

	void					postEvent(InputEvent event);

	// Convenience methods
	void					onSquareSelected(Square sq);
	void					onPromotionChosen(PieceType piece);
	void					onRemoteMoveReceived(Move move);
	void					onCPUMoveCalculated(Move move);
	void					onUndoRequested();
	void					onGameStart(GameConfiguration config);
	void					onGameReset();

	//=========================================================================
	// Queries
	//=========================================================================

	[[nodiscard]] GameState getState() const { return mState.load(); }

private:
	void				   run() override;

	//=========================================================================
	// Event Processing
	//=========================================================================

	void				   processEvent(const InputEvent &event);

	//=========================================================================
	// State Handlers - return next state
	//=========================================================================

	GameState			   handleInit(const InputEvent &event);
	GameState			   handleWaitingForInput(const InputEvent &event);
	GameState			   handleWaitingForTarget(const InputEvent &event);
	GameState			   handlePawnPromotion(const InputEvent &event);
	GameState			   handleWaitingForRemote(const InputEvent &event);
	GameState			   handleWaitingForCPU(const InputEvent &event);
	GameState			   handleGameOver(const InputEvent &event);

	//=========================================================================
	// Helpers
	//=========================================================================

	void				   transitionTo(GameState newState);
	GameState			   determineNextTurnState();
	bool				   tryExecuteMove(Move move, bool fromRemote);


	//=========================================================================
	// State
	//=========================================================================

	std::atomic<GameState> mState{GameState::Init};
	MoveIntent			   mMoveIntent;
	EndGameState		   mEndgameState{EndGameState::OnGoing};

	//=========================================================================
	// Mode Flags
	//=========================================================================

	std::atomic<bool>	   mIsMultiplayer{false};
	std::atomic<bool>	   mIsVsCPU{false};

	//=========================================================================
	// Dependencies (non-owning)
	//=========================================================================

	IGameController		  *mController{nullptr};
	IInputSource		  *mInputSource{nullptr};

	//=========================================================================
	// Thread-Safe Event Queue
	//=========================================================================

	std::queue<InputEvent> mEventQueue;
	std::mutex			   mQueueMutex;
};
