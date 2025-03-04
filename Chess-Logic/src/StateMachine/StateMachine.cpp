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

	if (mCurrentState == GameState::WaitingForInput) // Move initiated
	{
		mMoveStart	  = pos;
		mMoveStartSet = true;
	}
	switchToNextState();
}


void StateMachine::onPawnPromotionChosen(PieceType promotion)
{
	std::lock_guard<std::mutex> lock(mMutex);
	if (mCurrentState == GameState::PawnPromotion)
	{
		mPromotionChoice   = promotion;
		mAwaitingPromotion = false;
	}
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
			// Initialize the Local Game
			bool result = handleInitState(false);
			setInitialized(result);

			switchToNextState();

			break;
		}
		case GameState::WaitingForInput:
		{
			// Set the player
			// Calculate moves and wait for input
			handleWaitingForInputState();
			break;
		}
		case GameState::MoveInitiated:
		{
			// Display possible moves to UI
			handleMoveInitiatedState();
			break;
		}
		case GameState::WaitingForTarget:
		{
			// Waiting for move end / Target
			// Start validating the move
			handleWaitingForTargetState();
			break;
		}
		case GameState::ValidatingMove:
		{
			// Check whether the move is valid -> jump to move execution
			// Check whether the move is not a valid move (target square it same as start or not a valid square) -> jump to waiting for input
			handleValidatingMoveState();
			break;
		}
		case GameState::ExecutingMove:
		{
			// Execute the move
			handleExecutingMoveState();
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
		break;
	}
	case GameState::MoveInitiated:
	{

		break;
	}
	case GameState::WaitingForTarget:
	{

		break;
	}
	case GameState::ValidatingMove:
	{

		break;
	}
	case GameState::ExecutingMove:
	{

		break;
	}
	case GameState::PawnPromotion:
	{

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
	GameManager::GetInstance()->calculateAllMovesForPlayer();
}


bool StateMachine::handleMoveInitiatedState() {}


bool StateMachine::handleWaitingForTargetState() {}


bool StateMachine::handleValidatingMoveState() {}


bool StateMachine::handleExecutingMoveState() {}


bool StateMachine::handlePawnPromotionState() {}


bool StateMachine::handleGameOverState() {}
