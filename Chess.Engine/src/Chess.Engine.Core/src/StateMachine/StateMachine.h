/*
  ==============================================================================
	Module:         StateMachine
	Description:    Thread managing the game states
  ==============================================================================
*/

#pragma once

#include <mutex>
#include <atomic>
#include <condition_variable>

#include "Parameters.h"
#include "Move.h"
#include "IObservable.h"
#include "ThreadBase.h"
#include "CPUPLayer.h"

class GameManager;


/**
 * @brief Drives progression through distinct game states (init, input, validation,
 *        execution, promotion, remote/CPU wait, game over). Mediates UI, engine,
 *        CPU engine, and multiplayer events.
 *
 * Threaded: Inherits ThreadBase to process queued state transitions and asynchronous events.
 */
class StateMachine : public IGameStateObservable, public ThreadBase, public IRemoteMessagesObserver, public ICPUMoveObserver, public std::enable_shared_from_this<StateMachine>
{
public:
	static std::shared_ptr<StateMachine> GetInstance();
	static void							 ReleaseInstance();

	~StateMachine();

	/**
	 * @brief	Begin a new game using supplied configuration (UI trigger).
	 */
	void	  onGameStarted(GameConfiguration config);

	/**
	 * @brief	Start a multiplayer game session (UI trigger).
	 */
	void	  onMultiplayerGameStarted();

	/**
	 * @brief	UI square selection event (first selects source, second destination).
	 */
	void	  onSquareSelected(const Position &pos);

	/**
	 * @brief	UI selection of a promotion piece type.
	 */
	void	  onPawnPromotionChosen(PieceType promotion);

	/**
	 * @brief	Observer callback when an external component changes game state.
	 */
	void	  gameStateChanged(const GameState state) override;

	GameState getCurrentGameState() { return mCurrentState.load(); }
	void	  setCurrrentGameState(const GameState state) { mCurrentState.store(state); }

	void	  onRemoteMoveReceived(const PossibleMove &remoteMove) override;
	void	  onRemoteChatMessageReceived(const std::string &mesage) override {}
	void	  onRemoteConnectionStateReceived(const ConnectionState &state) override {}
	void	  onRemoteInvitationReceived(const InvitationRequest &invite) override {};
	void	  onRemoteInvitationResponseReceived(const InvitationResponse &response) override {};
	void	  onRemotePlayerChosenReceived(const PlayerColor player) override {};
	void	  onRemotePlayerReadyFlagReceived(const bool flag) override {};

	/**
	 * @brief	CPU engine produced a move. Consumed when entering WaitingForCPUMove -> ExecutingMove.
	 */
	void	  onMoveCalculated(PossibleMove cpuMove) override;

	/**
	 * @brief	Whether initial setup completed.
	 */
	bool	  isInitialized() const;

	/**
	 * @brief	Set initialization flag (internal).
	 */
	void	  setInitialized(const bool value);

	/**
	 * @brief	Reset all runtime state to start a fresh game.
	 */
	void	  resetGame();

	/**
	 * @brief	Handle user request to undo last move (transitions if valid).
	 */
	void	  reactToUndoMove();

private:
	StateMachine();

	/**
	 * @brief	Thread loop dispatching state handlers until stopped.
	 */
	void				   run() override;

	bool				   handleInitState() const;
	bool				   handleWaitingForInputState();
	bool				   handleMoveInitiatedState() const;
	bool				   handleWaitingForTargetState();
	bool				   handleValidatingMoveState();
	bool				   handleExecutingMoveState();
	bool				   handlePawnPromotionState();
	bool				   handleGameOverState();
	bool				   handleWaitingForRemoteState();
	bool				   handleWaitingForCPUState();

	/**
	 * @brief	Apply pending state transition (if requested).
	 */
	void				   switchToNextState();

	bool				   isGameOngoing() const { return mEndgameState == EndGameState::OnGoing; }

	/**
	 * @brief	Clear current in-progress move structure.
	 */
	void				   resetCurrentPossibleMove();


	std::atomic<bool>	   mInitialized{false};

	std::atomic<GameState> mCurrentState{GameState::Undefined};

	PossibleMove		   mCurrentPossibleMove{};

	bool				   mMovesCalulated{false};

	bool				   mMoveInitiated{false};
	bool				   mWaitingForTargetStart{false};
	bool				   mWaitingForTargetEnd{false};

	bool				   mIsValidMove{false};

	bool				   mAwaitingPromotion{false};
	bool				   mReceivedMoveFromRemote{false};

	EndGameState		   mEndgameState{EndGameState::OnGoing};

	std::atomic<bool>	   mIsMultiplayerGame{false};

	std::atomic<bool>	   mPlayingAgainstPC{false};
	bool				   mWaitingForCPUMove{false};

	std::mutex			   mStateChangedMutex;
	bool				   mHasPendingStateChange{false};
	GameState			   mPendingState{GameState::Undefined};
};
