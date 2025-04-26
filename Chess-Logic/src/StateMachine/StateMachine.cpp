/*
  ==============================================================================
	Module:         StateMachine
	Description:    Thread managing the game states
  ==============================================================================
*/


#include "StateMachine.h"
#include "GameManager.h"


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


StateMachine::StateMachine() {}


StateMachine::~StateMachine()
{
	stop();
}


void StateMachine::onGameStarted()
{
	if (getCurrentGameState() == GameState::Undefined)
	{
		gameStateChanged(GameState::Init);
		start();
		triggerEvent();
	}
}


void StateMachine::onMultiplayerGameStarted(bool isHost)
{
	mIsMultiplayerGame = true;
	mIsLocalHost	   = isHost;

	onGameStarted();
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
		mPromotionChoice   = promotion;
		mAwaitingPromotion = false;
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
	GameManager::GetInstance()->resetGame();
}


void StateMachine::run()
{
	while (isRunning())
	{
		waitForEvent();

		LOG_INFO("Processing state: {}", LoggingHelper::gameStateToString(getCurrentGameState()).c_str());

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
				bool result = handleInitState(mIsMultiplayerGame);
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
		gameStateChanged(GameState::WaitingForInput);
		break;
	}
	case GameState::WaitingForInput:
	{
		break;
	}
	case GameState::MoveInitiated:
	{
		gameStateChanged(GameState::WaitingForTarget);
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
			resetCurrentPossibleMove();
			mMovesCalulated		   = false;
			mWaitingForTargetStart = false;
			mWaitingForTargetEnd   = false;

			gameStateChanged(GameState::WaitingForInput);
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


bool StateMachine::handleInitState(bool multiplayer)
{
	LOG_INFO("Handling init state");

	if (multiplayer)
	{
		return GameManager::GetInstance()->startMultiplayerGame(mIsLocalHost);
	}
	else
	{
		return GameManager::GetInstance()->startGame();
	}
}


bool StateMachine::handleWaitingForInputState()
{
	LOG_INFO("Handling waiting for input state");

	resetCurrentPossibleMove();

	GameManager::GetInstance()->switchTurns(); // Sets the player

	mMovesCalulated = GameManager::GetInstance()->calculateAllMovesForPlayer();
	return mMovesCalulated;
}


bool StateMachine::handleMoveInitiatedState()
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
	GameManager::GetInstance()->executeMove(mCurrentPossibleMove);
	return true;
}


bool StateMachine::handlePawnPromotionState()
{
	// @TODO
	return false;
}


bool StateMachine::handleGameOverState()
{
	// @TODO: Let UI know of EndGameState -> Checkmate or Stalemate
	return false;
}
