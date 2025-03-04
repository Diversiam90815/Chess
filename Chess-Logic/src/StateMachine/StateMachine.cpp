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


void StateMachine::run()
{
	while (mRunning)
	{
		std::unique_lock<std::mutex> lock(mMutex);
		cv.wait_for(lock, std::chrono::milliseconds(50));

		switch (mCurrentState)
		{
		case GameState::InitLocal:
		{
			// Initialize the Local Game
			break;
		}
		case GameState::InitLAN:
		{
			// Initialize the LAN Game
			break;
		}
		case GameState::WaitingForInput:
		{
			// Set the player
			// Calculate moves and wait for input
			break;
		}
		case GameState::MoveInitiated:
		{
			// Display possible moves to UI
			break;
		}
		case GameState::WaitingForTarget:
		{
			// Waiting for move end / Target
			// Start validating the move
			break;
		}
		case GameState::ValidatingMove:
		{
			// Check whether the move is valid -> jump to move execution
			// Check whether the move is not a valid move (target square it same as start or not a valid square) -> jump to waiting for input
			break;
		}
		case GameState::ExecutingMove:
		{
			// Execute the move
			break;
		}
		case GameState::PawnPromotion:
		{
			// Wait for pawn promotion piece to be received from UI and then execute
			break;
		}
		case GameState::GameOver:
		{
			// Determine the EndGameState and send message to UI
			break;
		}
		default: break;
		}
	}
}
