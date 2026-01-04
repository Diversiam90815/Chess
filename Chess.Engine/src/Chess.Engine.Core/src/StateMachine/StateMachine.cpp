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

	// TODO: delegate state handling
}


GameState StateMachine::handleInit(const InputEvent &event)
{
	return GameState();
}


GameState StateMachine::handleWaitingForInput(const InputEvent &event)
{
	return GameState();
}


GameState StateMachine::handleWaitingForTarget(const InputEvent &event)
{
	return GameState();
}


GameState StateMachine::handlePawnPromotion(const InputEvent &event)
{
	return GameState();
}


GameState StateMachine::handleWaitingForRemote(const InputEvent &event)
{
	return GameState();
}


GameState StateMachine::handleWaitingForCPU(const InputEvent &event)
{
	return GameState();
}


GameState StateMachine::handleGameOver(const InputEvent &event)
{
	return GameState();
}


void	  StateMachine::transitionTo(GameState newState) {}


GameState StateMachine::determineNextTurnState()
{
	return GameState();
}


bool StateMachine::tryExecuteMove(Move move, bool fromRemote)
{
	return false;
}
