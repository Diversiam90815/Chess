/*
  ==============================================================================
	Module:         StateMachine
	Description:    Thread managing the game states
  ==============================================================================
*/

#include "StateMachine.h"


StateMachine::~StateMachine()
{
	stop();
}


void StateMachine::setGameController(IGameController *controller)
{
	mController = controller;
}


void StateMachine::setInputSource(IInputSource *source)
{
	mInputSource = source;
}


void StateMachine::postEvent(InputEvent event)
{
	{
		std::lock_guard<std::mutex> lock(mQueueMutex);
		mEventQueue.push(std::move(event));
	}
	mQueueCV.notify_one();
}


void StateMachine::onSquareSelected(Square sq)
{
	postEvent(InputEvent::SquareSelected(sq));
}


void StateMachine::onPromotionChosen(PieceTypes piece)
{
	postEvent(InputEvent::PromotionChosen(piece));
}


void StateMachine::onRemoteMoveReceived(Move move)
{
	postEvent(InputEvent::RemoteMove(move));
}


void StateMachine::onCPUMoveCalculated(Move move)
{
	postEvent(InputEvent::CPUMove(move));
}


void StateMachine::onUndoRequested()
{
	postEvent(InputEvent::Undo());
}


void StateMachine::onGameStart(GameConfiguration config)
{
	postEvent(InputEvent::Start(config));
}


void StateMachine::onGameReset()
{
	postEvent(InputEvent::Reset());
}


void StateMachine::run()
{
	while (isRunning())
	{
		InputEvent event;

		{
			std::unique_lock lock(mQueueMutex);
			mQueueCV.wait(lock, [this]() { return !mEventQueue.empty() || !isRunning(); });

			if (!isRunning())
				break;

			event = std::move(mEventQueue.front());
			mEventQueue.pop();
		}

		processEvent(event);
	}
}


void StateMachine::processEvent(const InputEvent &event)
{
	if (!mController || !mInputSource)
	{
		LOG_ERROR("Controller or InputSource not set!");
		return;
	}

	GameState currentState = mState.load();
	GameState nextState	   = currentState;

	switch (currentState)
	{
	case GameState::Init:
	case GameState::Undefined: nextState = handleInit(event); break;

	case GameState::WaitingForInput: nextState = handleWaitingForInput(event); break;

	case GameState::WaitingForTarget: nextState = handleWaitingForTarget(event); break;

	case GameState::PawnPromotion: nextState = handlePawnPromotion(event); break;

	case GameState::WaitingForRemoteMove: nextState = handleWaitingForRemote(event); break;

	case GameState::WaitingForCPUMove: nextState = handleWaitingForCPU(event); break;

	case GameState::GameOver: nextState = handleGameOver(event); break;

	default: break;
	}

	if (nextState != currentState)
		transitionTo(nextState);
}


GameState StateMachine::handleInit(const InputEvent &event)
{
	if (event.type == InputEvent::Type::GameStart)
	{
		mIsMultiplayer.store(event.config.mode == GameModeSelection::Multiplayer);
		mIsVsCPU.store(event.config.mode == GameModeSelection::VsCPU);

		if (!mController->initializeGame(event.config))
		{
			LOG_ERROR("Failed to initialize game!");
			return GameState::Init;
		}

		mMoveIntent.clear();
		mEndgameState = EndGameState::OnGoing;

		return determineNextTurnState();
	}

	return GameState::Init;
}


GameState StateMachine::handleWaitingForInput(const InputEvent &event)
{
	switch (event.type)
	{
	case InputEvent::Type::SquareSelected:
	{
		MoveList moves;
		mController->getLegalMovesFromSquare(event.square, moves);

		if (moves.size() > 0)
		{
			mMoveIntent.clear();
			mMoveIntent.fromSquare = event.square;
			mMoveIntent.legalMoves = moves;

			mInputSource->onLegalMovesAvailable(event.square, moves);
			return GameState::WaitingForTarget;
		}
		break;
	}
	case InputEvent::Type::UndoRequested:
	{
		if (mController->undoLastMove())
		{
			mInputSource->onMoveUndone();
			mInputSource->onBoardStateChanged();
		}
		break;
	}
	case InputEvent::Type::GameReset:
	{
		mController->resetGame();
		mMoveIntent.clear();
		return GameState::Init;
	}
	default: break;
	}

	return GameState::WaitingForInput;
}


GameState StateMachine::handleWaitingForTarget(const InputEvent &event)
{
	if (event.type != InputEvent::Type::SquareSelected)
		return GameState::WaitingForTarget;

	Square sq = event.square;

	// Deselect if clicked on same square
	if (sq == mMoveIntent.fromSquare)
	{
		mMoveIntent.clear();
		return GameState::WaitingForInput;
	}

	// Clicked different own piece -> reselect
	MoveList newMoves;
	mController->getLegalMovesFromSquare(sq, newMoves);

	if (newMoves.size() > 0)
	{
		mMoveIntent.clear();
		mMoveIntent.fromSquare = sq;
		mMoveIntent.legalMoves = newMoves;

		mInputSource->onLegalMovesAvailable(sq, newMoves);
		return GameState::WaitingForTarget;
	}

	// try target square
	mMoveIntent.toSquare = sq;

	// Check if promotion is needed
	if (mController->isPromotionMove(mMoveIntent.fromSquare, mMoveIntent.toSquare))
	{
		mInputSource->onPromotionRequired();
		return GameState::PawnPromotion;
	}

	// Find and execute move
	Move move = mController->findMove(mMoveIntent.fromSquare, mMoveIntent.toSquare);

	if (move.isValid() && tryExecuteMove(move, false))
		return determineNextTurnState();

	// invalid move
	mMoveIntent.clear();
	return GameState::WaitingForInput;
}


GameState StateMachine::handlePawnPromotion(const InputEvent &event)
{
	if (event.type == InputEvent::Type::PromotionChosen)
	{
		mMoveIntent.promotion = event.promotion;

		Move move			  = mController->findMove(mMoveIntent.fromSquare, mMoveIntent.toSquare, mMoveIntent.promotion);

		if (move.isValid() && tryExecuteMove(move, false))
			return determineNextTurnState();

		mMoveIntent.clear();
		return GameState::WaitingForInput;
	}

	return GameState::PawnPromotion;
}


GameState StateMachine::handleWaitingForRemote(const InputEvent &event)
{
	if (event.type == InputEvent::Type::RemoteMove)
	{
		if (tryExecuteMove(event.move, true))
			return determineNextTurnState();
	}

	return GameState::WaitingForRemoteMove;
}


GameState StateMachine::handleWaitingForCPU(const InputEvent &event)
{
	if (event.type == InputEvent::Type::CPUMove)
	{
		if (tryExecuteMove(event.move, false))
			return determineNextTurnState();

		LOG_ERROR("CPU returned invalid move");
		return GameState::GameOver;
	}

	mController->requestCPUMoveAsync();

	return GameState::WaitingForCPUMove;
}


GameState StateMachine::handleGameOver(const InputEvent &event)
{
	if (event.type == InputEvent::Type::GameReset)
	{
		mController->resetGame();
		mMoveIntent.clear();
		mEndgameState = EndGameState::OnGoing;
		return GameState::Init;
	}

	return GameState::GameOver;
}


void StateMachine::transitionTo(GameState newState)
{
	GameState oldState = mState.exchange(newState);

	LOG_INFO("State transition: {} -> {}", LoggingHelper::gameStateToString(oldState), LoggingHelper::gameStateToString(newState));

	if (mInputSource)
		mInputSource->onGameStateChanged(newState);
}


GameState StateMachine::determineNextTurnState()
{
	if (mEndgameState != EndGameState::OnGoing)
	{
		Side winner = (mEndgameState == EndGameState::Checkmate) ? (mController->getCurrentSide() == Side::White ? Side::Black : Side::White) : Side::None;

		if (mInputSource)
			mInputSource->onGameEnded(mEndgameState, winner);

		return GameState::GameOver;
	}

	if (mIsMultiplayer.load() && !mController->isLocalPlayerTurn())
		return GameState::WaitingForRemoteMove;

	if (mIsVsCPU.load() && mController->isCPUTurn())
		return GameState::WaitingForCPUMove;

	return GameState::WaitingForInput;
}


bool StateMachine::tryExecuteMove(Move move, bool fromRemote)
{
	if (!mController->executeMove(move, fromRemote))
		return false;

	mInputSource->onMoveExecuted(move, fromRemote);
	mInputSource->onBoardStateChanged();

	mController->switchTurns();
	mEndgameState = mController->checkEndGame();

	mMoveIntent.clear();
	return true;
}
