/*
  ==============================================================================
	Module:         StateMachine
	Description:    Thread managing the game states
  ==============================================================================
*/


#include "StateMachine.h"


StateMachine *StateMachine::GetInstance()
{
	static StateMachine *sInstance = nullptr;
	if (nullptr == sInstance)
	{
		sInstance = new StateMachine();
	}
	return sInstance;
}


void StateMachine::ReleaseInstance()
{
	StateMachine *sInstance = GetInstance();
	if (sInstance)
	{
		delete sInstance;
	}
}


StateMachine::StateMachine() : mRunning(false) {}


StateMachine::~StateMachine()
{
	stop();
}


void StateMachine::start()
{
	mRunning.store(true);
	worker = boost::thread(&StateMachine::run, this);
}


void StateMachine::stop()
{
	{
		std::lock_guard<std::mutex> lock(mMutex);
		mRunning.store(false);
		cv.notify_all();
	}

	if (worker.joinable())
	{
		worker.join();
	}
}


void StateMachine::onGameStarted()
{
	if (mCurrentState == GameState::Undefined)
	{
		switchToNextState();
	}
}


void StateMachine::onSquareSelected(const Position &pos)
{
	std::lock_guard<std::mutex> lock(mMutex);

	if (mCurrentState == GameState::WaitingForInput) // Selected Start Position
	{
		mMoveStart = pos;
		switchToNextState();
	}
	else if (mCurrentState == GameState::WaitingForTarget) // Selected End Position
	{
		mMoveEnd = pos;
		switchToNextState();
	}
}


void StateMachine::onPawnPromotionChosen(PieceType promotion)
{
	std::lock_guard<std::mutex> lock(mMutex);
	if (mCurrentState == GameState::PawnPromotion)
	{
		mPromotionChoice   = promotion;
		mAwaitingPromotion = false;
	}
	setGameState(GameState::ExecutingMove);
	cv.notify_all();
}


void StateMachine::setGameState(const GameState state)
{
	if (mCurrentState != state)
	{
		mCurrentState = state;
	}
}


GameState StateMachine::getGameState() const
{
	return mCurrentState;
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


void StateMachine::run()
{
	while (mRunning)
	{
		std::unique_lock<std::mutex> lock(mMutex);
		cv.wait_for(lock, std::chrono::milliseconds(100)); // Runs through every 100ms in absence of any notification

		switch (mCurrentState)
		{
		case GameState::Undefined:
		{
			break;
		}
		case GameState::Init:
		{
			if (!isInitialized())
			{
				bool result = handleInitState(false); // Initialize the Local Game
				setInitialized(result);
			}

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
			handleMoveInitiatedState();
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


bool StateMachine::switchToNextState()
{
	bool stateChanged = false;

	switch (mCurrentState)
	{
	case GameState::Undefined:
	{
		setGameState(GameState::Init);
		stateChanged = true;
		break;
	}
	case GameState::Init:
	{
		if (isInitialized())
		{
			setGameState(GameState::WaitingForInput);
			stateChanged = true;
		}
		break;
	}

	case GameState::WaitingForInput:
	{
		// Change to MoveInitiated happening in onSquareSelected
		break;
	}
	case GameState::MoveInitiated:
	{
		setGameState(GameState::WaitingForTarget);
		stateChanged = true;
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
			PossibleMove tmpMove{};
			tmpMove.start		 = mMoveStart;
			tmpMove.end			 = mMoveEnd;
			bool isPawnPromotion = GameManager::GetInstance()->checkForPawnPromotionMove(tmpMove);

			if (isPawnPromotion)
				setGameState(GameState::PawnPromotion);
			else
				setGameState(GameState::ExecutingMove);
		}
		else
		{
			mMoveStart			 = {};
			mMoveEnd			 = {};
			mWaitingForTargetEnd = false;
			setGameState(GameState::WaitingForInput);
		}
		stateChanged = true;
		break;
	}
	case GameState::ExecutingMove:
	{
		mEndgameState = GameManager::GetInstance()->checkForEndGameConditions();
		if (isGameOngoing())
		{
			mMoveStart			   = {};
			mMoveEnd			   = {};
			mMovesCalulated		   = false;
			mWaitingForTargetStart = false;
			mWaitingForTargetEnd   = false;

			setGameState(GameState::WaitingForInput);
		}
		else
		{
			setGameState(GameState::GameOver);
		}

		stateChanged = true;
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

	if (stateChanged)
	{
		cv.notify_all();
	}
	return stateChanged;
}


bool StateMachine::handleInitState(bool multiplayer)
{
	// currently impl with no multiplayer
	bool result = GameManager::GetInstance()->init();

	if (result)
	{
		GameManager::GetInstance()->startGame();
	}

	return result;
}


bool StateMachine::handleWaitingForInputState()
{
	GameManager::GetInstance()->switchTurns(); // Sets the player
	mMovesCalulated = GameManager::GetInstance()->calculateAllMovesForPlayer();
	return mMovesCalulated;
}


bool StateMachine::handleMoveInitiatedState()
{
	bool result = GameManager::GetInstance()->initiateMove(mMoveStart);
	return result;
}


bool StateMachine::handleWaitingForTargetState()
{
	GameManager::GetInstance()->moveStateInitiated();
	return true;
}


bool StateMachine::handleValidatingMoveState()
{
	PossibleMove tmpMove{};
	tmpMove.start = mMoveStart;
	tmpMove.end	  = mMoveEnd;

	mIsValidMove  = GameManager::GetInstance()->checkForValidMoves(tmpMove);
	return mIsValidMove;
}


bool StateMachine::handleExecutingMoveState()
{
	PossibleMove tmpMove{};
	tmpMove.start = mMoveStart;
	tmpMove.end	  = mMoveEnd;
	GameManager::GetInstance()->executeMove(tmpMove);
	return true;
}


bool StateMachine::handlePawnPromotionState()
{
	return false;
}


bool StateMachine::handleGameOverState()
{
	// Let UI know of EndGameState -> Checkmate or Stalemate
	return false;
}
