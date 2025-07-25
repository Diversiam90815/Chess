/*
  ==============================================================================
	Module:         StateMachine
	Description:    Thread managing the game states
  ==============================================================================
*/


#include "StateMachine.h"
#include "GameManager.h"


std::shared_ptr<StateMachine> StateMachine::GetInstance()
{
	static std::shared_ptr<StateMachine> sInstance(new StateMachine());
	return sInstance;
}


void StateMachine::ReleaseInstance()
{
	auto sInstance = GetInstance();
	if (sInstance)
	{
		sInstance = nullptr;
	}
}


StateMachine::StateMachine() {}


StateMachine::~StateMachine()
{
	stop();
}


void StateMachine::onGameStarted(GameConfiguration config)
{
	GameManager::GetInstance()->setGameConfiguration(config);

	if (config.mode == GameModeSelection::VsCPU)
	{
		LOG_INFO("Starting a game against the CPU!");
		mPlayingAgainstPC.store(true);
	}
	else
	{
		mPlayingAgainstPC.store(false);
	}

	GameState currentState = getCurrentGameState();

	if (currentState == GameState::Undefined)
	{
		gameStateChanged(GameState::Init);
		start();
		triggerEvent();
	}
	else
	{
		int iCurrentState = static_cast<int>(currentState);

		LOG_WARNING("Game Start called, but our state is wrong/not set up! Our current state is {0} ({1})", LoggingHelper::gameStateToString(currentState).c_str(), iCurrentState);
	}
}


void StateMachine::onMultiplayerGameStarted()
{
	LOG_INFO("Starting a Multiplayer Game!");
	mIsMultiplayerGame.store(true);

	onGameStarted({});
}


void StateMachine::onSquareSelected(const Position &pos)
{
	if (getCurrentGameState() == GameState::WaitingForInput) // Selected Start Position
	{
		mCurrentPossibleMove.start = pos;
		gameStateChanged(GameState::MoveInitiated);
	}
	else if (getCurrentGameState() == GameState::WaitingForTarget) // Selected End Position
	{
		mCurrentPossibleMove.end = pos;
		gameStateChanged(GameState::ValidatingMove);
	}
}


void StateMachine::onPawnPromotionChosen(PieceType promotion)
{
	if (getCurrentGameState() == GameState::PawnPromotion)
	{
		mCurrentPossibleMove.promotionPiece = promotion;
		mAwaitingPromotion					= false;
	}
	gameStateChanged(GameState::ExecutingMove);
}


void StateMachine::gameStateChanged(const GameState state)
{
	setCurrrentGameState(state);

	LOG_INFO("Game State changed to : {}", LoggingHelper::gameStateToString(state).c_str());

	for (auto &observer : mObservers)
	{
		auto obs = observer.lock();

		if (obs)
			obs->onGameStateChanged(state);
	}
	triggerEvent();
}


void StateMachine::onRemoteMoveReceived(const PossibleMove &remoteMove)
{
	if (getCurrentGameState() == GameState::WaitingForRemoteMove)
	{
		LOG_INFO("Remote move received!");

		// Setting the current move
		mCurrentPossibleMove	= remoteMove;

		mReceivedMoveFromRemote = true; // Set flag to true to avoid echo effect

		// Check if a pawn promotion is needed!
		bool isPawnPromotion	= GameManager::GetInstance()->checkForPawnPromotionMove(mCurrentPossibleMove);

		if (isPawnPromotion && mCurrentPossibleMove.promotionPiece == PieceType::DefaultType)
		{
			LOG_ERROR("Remote move requires promotion, however no promotion piece has been specified!");
			return;
		}

		// We will enter the execute move state!

		{
			std::lock_guard<std::mutex> lock(mStateChangedMutex);
			mHasPendingStateChange = true;
			mPendingState		   = GameState::ExecutingMove;
		}
		triggerEvent();
	}

	else
	{
		LOG_WARNING("Received a move from the remote endpoint, but we are not in the wrong state! Our state is {}",
					LoggingHelper::gameStateToString(getCurrentGameState()).c_str());
	}
}


void StateMachine::onMoveCalculated(PossibleMove cpuMove)
{
	if (!cpuMove.isEmpty())
	{
		LOG_INFO("CPU selcted move from {} to {}", LoggingHelper::positionToString(cpuMove.start).c_str(), LoggingHelper::positionToString(cpuMove.end).c_str());

		mReceivedMoveFromRemote = true; // treat CPU as remote so the move will be initiated correctly before execution
		// Set move and transition to validation
		mCurrentPossibleMove	= cpuMove;
		mWaitingForCPUMove		= false;
		gameStateChanged(GameState::ExecutingMove);
	}
	else
	{
		LOG_ERROR("CPU returned invalid move");
		mWaitingForCPUMove = false;
		gameStateChanged(GameState::GameOver);
	}
}


bool StateMachine::isInitialized() const
{
	return mInitialized.load();
}


void StateMachine::setInitialized(const bool value)
{
	if (mInitialized.load() != value)
	{
		mInitialized.store(value);
	}
}


void StateMachine::resetGame()
{
	mWaitingForTargetStart = false;
	mWaitingForTargetEnd   = false;
	resetCurrentPossibleMove();
	setCurrrentGameState(GameState::Undefined);
	setInitialized(false);
	GameManager::GetInstance()->resetGame();
}


void StateMachine::run()
{
	while (isRunning())
	{
		waitForEvent();

		LOG_INFO("Processing state: {}", LoggingHelper::gameStateToString(getCurrentGameState()).c_str());

		// Check for pending state changes
		GameState pendingState;
		bool	  hasStateChange = false;

		{
			std::lock_guard<std::mutex> stateLock(mStateChangedMutex);
			if (mHasPendingStateChange)
			{
				pendingState		   = mPendingState;
				mHasPendingStateChange = false;
				hasStateChange		   = true;
			}
		}

		if (hasStateChange)
		{
			// Process the state change, call stateChanged
			gameStateChanged(pendingState);
		}

		// Process the current state
		switch (getCurrentGameState())
		{
		case GameState::Undefined:
		{
			switchToNextState();
			break;
		}
		case GameState::Init:
		{
			if (!isInitialized())
			{
				bool result = handleInitState();
				setInitialized(result);
			}

			switchToNextState();

			break;
		}
		case GameState::InitSucceeded:
		{
			switchToNextState();
			break;
		}
		case GameState::WaitingForInput:
		{
			// Set the player
			// Calculate moves and wait for input

			if (!mWaitingForTargetStart)
			{
				mWaitingForTargetStart = handleWaitingForInputState();
			}

			// Switch to next state happening in onSquareSelected
			break;
		}
		case GameState::MoveInitiated:
		{
			// Display possible moves to UI
			mMoveInitiated = handleMoveInitiatedState();
			switchToNextState();
			break;
		}
		case GameState::WaitingForTarget:
		{
			// Waiting for move end / Target
			// Start validating the move
			if (!mWaitingForTargetEnd)
			{
				bool result			 = handleWaitingForTargetState();
				mWaitingForTargetEnd = result;
			}

			// Change to ValidatingMove happening in onSquareSelected
			break;
		}
		case GameState::ValidatingMove:
		{
			// Check whether the move is valid -> jump to move execution
			// Check whether the move is not a valid move (target square it same as start or not a valid square) -> jump to waiting for input
			handleValidatingMoveState();
			switchToNextState();
			break;
		}
		case GameState::ExecutingMove:
		{
			// Execute the move
			handleExecutingMoveState();
			switchToNextState();
			break;
		}
		case GameState::PawnPromotion:
		{
			// Wait for pawn promotion piece to be received from UI and then execute
			handlePawnPromotionState();
			break;
		}
		case GameState::WaitingForRemoteMove:
		{
			handleWaitingForRemoteState();
			break;
		}
		case GameState::WaitingForCPUMove:
		{
			handleWaitingForCPUState();
			break;
		}
		case GameState::GameOver:
		{
			// Determine the EndGameState and send message to UI
			handleGameOverState();
			break;
		}
		default: break;
		}
	}
}


void StateMachine::switchToNextState()
{
	switch (getCurrentGameState())
	{
	case GameState::Undefined:
	{
		gameStateChanged(GameState::Init);
		break;
	}
	case GameState::Init:
	{
		if (isInitialized())
		{
			gameStateChanged(GameState::InitSucceeded);
		}
		break;
	}
	case GameState::InitSucceeded:
	{
		if (mIsMultiplayerGame.load())
		{
			bool isLocalPlayerTurn = GameManager::GetInstance()->isLocalPlayerTurn();
			if (isLocalPlayerTurn)
			{
				LOG_INFO("Initial State: Local Player's turn");
				gameStateChanged(GameState::WaitingForInput);
			}
			else
			{
				LOG_INFO("Initial State: Remote Player's turn, waiting for remote move");
				gameStateChanged(GameState::WaitingForRemoteMove);
			}
		}
		else if (mPlayingAgainstPC.load())
		{
			bool isCPUTurn = GameManager::GetInstance()->isCPUPlayer(PlayerColor::White); // White always goes first
			if (isCPUTurn)
			{

				LOG_INFO("Initial state: CPU's turn");
				gameStateChanged(GameState::WaitingForCPUMove);
			}
			else
			{
				LOG_INFO("Initial state: Local Player's turn");
				gameStateChanged(GameState::WaitingForInput);
			}
		}
		else if (mIsCPUvsCPU.load())
		{
			LOG_INFO("Initial state: CPU vs CPU mode - White CPU starts");
			gameStateChanged(GameState::WaitingForCPUMove);
		}
		else
		{
			LOG_INFO("Single Player mode. We switch to WaitingForInput");
			gameStateChanged(GameState::WaitingForInput);
		}
		break;
	}
	case GameState::WaitingForInput:
	{
		break;
	}
	case GameState::MoveInitiated:
	{
		if (mMoveInitiated)
			gameStateChanged(GameState::WaitingForTarget);
		else
			gameStateChanged(GameState::WaitingForInput);

		break;
	}
	case GameState::WaitingForTarget:
	{
		// Change to ValidatingMove happening in onSquareSelected
		break;
	}
	case GameState::ValidatingMove:
	{
		if (mIsValidMove)
		{
			bool isPawnPromotion = GameManager::GetInstance()->checkForPawnPromotionMove(mCurrentPossibleMove);

			if (isPawnPromotion)
				gameStateChanged(GameState::PawnPromotion);
			else
				gameStateChanged(GameState::ExecutingMove);
		}
		else
		{
			resetCurrentPossibleMove();
			mWaitingForTargetEnd = false;
			gameStateChanged(GameState::WaitingForInput);
		}
		break;
	}
	case GameState::ExecutingMove:
	{
		mEndgameState = GameManager::GetInstance()->checkForEndGameConditions();

		if (isGameOngoing())
		{
			GameManager::GetInstance()->switchTurns();

			resetCurrentPossibleMove();
			mMovesCalulated			  = false;
			mWaitingForTargetStart	  = false;
			mWaitingForTargetEnd	  = false;

			PlayerColor currentPlayer = GameManager::GetInstance()->getCurrentPlayer();
			bool		isCPUTurn	  = GameManager::GetInstance()->isCPUPlayer(currentPlayer);

			// If we're in multiplayer mode, check who's turn it is
			if (mIsMultiplayerGame.load())
			{
				bool isLocalPlayerTurn = GameManager::GetInstance()->isLocalPlayerTurn();
				if (isLocalPlayerTurn)
				{
					LOG_INFO("Local Player's turn, so we wait for the input!");
					gameStateChanged(GameState::WaitingForInput);
				}
				else
				{
					LOG_INFO("Remote Player's turn, so we wait until we get a move from the remote!");
					// If it's not local player's turn, switch to WaitForRemoteMove state!
					gameStateChanged(GameState::WaitingForRemoteMove);
				}
			}
			else if (mPlayingAgainstPC.load() && isCPUTurn)
			{
				LOG_INFO("CPU PLayer's turn, waiting for CPU move");
				gameStateChanged(GameState::WaitingForCPUMove);
			}
			else if (mIsCPUvsCPU.load())
			{
				// In CPU vs CPU mode, always wait for CPU move regardless of color
				LOG_INFO("CPU vs CPU mode: {} CPU's turn, waiting for CPU move", LoggingHelper::playerColourToString(currentPlayer).c_str());
				gameStateChanged(GameState::WaitingForCPUMove);
			}
			else
			{
				LOG_INFO("Human player's turn, waiting for input");
				gameStateChanged(GameState::WaitingForInput);
			}
		}
		else
		{
			gameStateChanged(GameState::GameOver);
		}
		break;
	}
	case GameState::PawnPromotion:
	{
		// Change to Execute Move happening in onPawnPromotionChosen
		break;
	}
	case GameState::GameOver:
	{
		break;
	}
	default: break;
	}
}


void StateMachine::resetCurrentPossibleMove()
{
	LOG_INFO("Resetting the temporary saved possible move");
	mCurrentPossibleMove.start = {};
	mCurrentPossibleMove.end   = {};
	mCurrentPossibleMove.type  = MoveType::Normal;
}


void StateMachine::reactToUndoMove()
{
	// Remove the last move from the history and board
	GameManager::GetInstance()->undoMove();
	GameManager::GetInstance()->switchTurns(); // Switch back to the old player

	// Recalculate the new state (Changing player happening in WaitingForInputState)
	mWaitingForTargetStart = false;
	gameStateChanged(GameState::WaitingForInput);
}


bool StateMachine::handleInitState() const
{
	LOG_INFO("Handling init state");

	if (mIsMultiplayerGame)
		return GameManager::GetInstance()->startMultiplayerGame();

	else if (mPlayingAgainstPC)
		return GameManager::GetInstance()->startCPUGame();

	else if (isCPUvsCPUGame())
		return GameManager::GetInstance()->startCPUvsCPUGame();

	else
		return GameManager::GetInstance()->startGame();
}


bool StateMachine::handleWaitingForInputState()
{
	LOG_INFO("Handling waiting for input state");

	resetCurrentPossibleMove();

	mMovesCalulated = GameManager::GetInstance()->calculateAllMovesForPlayer();
	return mMovesCalulated;
}


bool StateMachine::handleMoveInitiatedState() const
{
	LOG_INFO("Handling move initiated state");
	bool result = GameManager::GetInstance()->initiateMove(mCurrentPossibleMove.start);
	return result;
}


bool StateMachine::handleWaitingForTargetState()
{
	return true;
}


bool StateMachine::handleValidatingMoveState()
{
	LOG_INFO("Validating move");
	mIsValidMove = GameManager::GetInstance()->checkForValidMoves(mCurrentPossibleMove);
	return mIsValidMove;
}


bool StateMachine::handleExecutingMoveState()
{
	GameManager::GetInstance()->executeMove(mCurrentPossibleMove, mReceivedMoveFromRemote);
	mReceivedMoveFromRemote = false; // Reset remote flag
	return true;
}


bool StateMachine::handlePawnPromotionState()
{
	mAwaitingPromotion = true;
	return true;
}


bool StateMachine::handleGameOverState()
{
	// Endgame state sent to UI via GameManager::checkForEndGameConditions
	// We should reset and close the StateMachine

	// resetGame();
	// this->stop();
	return true;
}


bool StateMachine::handleWaitingForRemoteState()
{
	// State change will happen in onRemoteMoveReceived()
	// but to validate the remote's move, we want to calculate the possible moves for the remote

	return GameManager::GetInstance()->calculateAllMovesForPlayer();
}


bool StateMachine::handleWaitingForCPUState()
{
	if (!mWaitingForCPUMove)
	{
		PlayerColor currentPlayer = GameManager::GetInstance()->getCurrentPlayer();

		// Start CPU Move calculation async
		GameManager::GetInstance()->requestCPUMoveAsync(currentPlayer);
		mWaitingForCPUMove = true;

		LOG_INFO("Started CPU move calculation for player {}", LoggingHelper::playerColourToString(currentPlayer).c_str());
	}

	return true;
}


void StateMachine::onCPUvsCPUGameStarted(CPUConfiguration whiteCPU, CPUConfiguration blackCPU)
{
	LOG_INFO("Starting CPU vs CPU game for testing purposes!");

	// Set CPU vs CPU mode
	setCPUvsCPU(true);
	mPlayingAgainstPC.store(false);
	mIsMultiplayerGame.store(false);

	// Configure both CPU players through GameManager
	GameManager::GetInstance()->setWhiteCPUConfiguration(whiteCPU);
	GameManager::GetInstance()->setBlackCPUConfiguration(blackCPU);

	GameState currentState = getCurrentGameState();

	if (currentState == GameState::Undefined)
	{
		gameStateChanged(GameState::Init);
		start();
		triggerEvent();
	}
	else
	{
		int iCurrentState = static_cast<int>(currentState);
		LOG_WARNING("CPU vs CPU Game Start called, but our state is wrong/not set up! Our current state is {0} ({1})", LoggingHelper::gameStateToString(currentState).c_str(),
					iCurrentState);
	}
}