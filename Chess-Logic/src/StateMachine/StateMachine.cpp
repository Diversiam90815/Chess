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
		mCurrentPossibleMove.promotionPiece = promotion;
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


void StateMachine::onRemoteMoveReceived(const PossibleMove &remoteMove)
{
	if (getCurrentGameState() == GameState::WaitingForRemoteMove)
	{
		LOG_INFO("Remote move received!");

		// Setting the current move
		mCurrentPossibleMove = remoteMove;

		// Is it valid?
		mIsValidMove		 = GameManager::GetInstance()->checkForValidMoves(mCurrentPossibleMove);

		if (!mIsValidMove)
		{
			LOG_WARNING("Invalid remote move received! This could indicate synchronisation issues!");
			resetCurrentPossibleMove();
			return;
		}

		// Check if a pawn promotion is needed!
		bool isPawnPromotion = GameManager::GetInstance()->checkForPawnPromotionMove(mCurrentPossibleMove);

		if (isPawnPromotion && mCurrentPossibleMove.promotionPiece == PieceType::DefaultType)
		{
			LOG_ERROR("Remote move requires promotion, however no promotion piece has been specified!");
			return;
		}

		// If the move is valid and set correctly, we will enter the execute move state!

		{
			std::lock_guard<std::mutex> lock(mStateChangedMutex);
			mHasPendingStateChange = true;
			mPendingState		   = GameState::ExecutingMove;
		}
		triggerEvent();
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
		case GameState::WaitingForRemoteMove:
		{
			// This state is passive -> we are waiting for onREmoteMoveReceieved to be called
			// Maybe implement a timeout later? (-> TODO?)
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

			// If we're in multiplayer mode, check who's turn it is
			if (mIsMultiplayerGame)
			{
				bool isLocalPlayerTurn = GameManager::GetInstance()->isLocalPlayerTurn();
				if (isLocalPlayerTurn)
				{
					gameStateChanged(GameState::WaitingForInput);
				}
				else
				{
					// If it's not local player's turn, switch to WaitForRemoteMove state!
					gameStateChanged(GameState::WaitingForRemoteMove);
				}
			}

			// Single player moves always switch to Waiting For Input!
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


bool StateMachine::handleInitState(bool multiplayer) const
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
	GameManager::GetInstance()->executeMove(mCurrentPossibleMove);
	return true;
}


bool StateMachine::handlePawnPromotionState()
{
	mAwaitingPromotion = true;
	return true;
}


bool StateMachine::handleGameOverState()
{
	// @TODO: Let UI know of EndGameState -> Checkmate or Stalemate
	return false;
}
