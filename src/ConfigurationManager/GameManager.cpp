/*
  ==============================================================================

	Class:          GameManager

	Description:    Manager for the Chess game

  ==============================================================================
*/

#include "GameManager.h"


GameManager::GameManager()
{
}


GameManager::~GameManager()
{
}


void GameManager::init()
{
	mMovementManager = std::make_unique<MovementManager>();

	mWhitePlayer.setPlayerColor(PlayerColor::White);
	mBlackPlayer.setPlayerColor(PlayerColor::Black);

	mWhitePlayer.setOnTurn(true);
	mCurrentPlayer = PlayerColor::White;
}


void GameManager::switchTurns()
{
	setCurrentMoveState(MoveState::NoMove);

	if (mCurrentPlayer == PlayerColor::White)
	{
		mWhitePlayer.setOnTurn(false);
		mBlackPlayer.setOnTurn(true);
		mCurrentPlayer = PlayerColor::Black;
		return;
	}
	mBlackPlayer.setOnTurn(false);
	mWhitePlayer.setOnTurn(true);
	mCurrentPlayer = PlayerColor::White;
}


void GameManager::executeMove(PossibleMove &move)
{
	Move executedMove = mMovementManager->executeMove(move);

	if (executedMove.capturedPiece != PieceType::DefaultType)
	{
		if (mCurrentPlayer == PlayerColor::White)
		{
			mWhitePlayer.addCapturedPiece(executedMove.capturedPiece);
			mBlackPlayer.updateScore();
		}
		else
		{
			mBlackPlayer.addCapturedPiece(executedMove.capturedPiece);
			mWhitePlayer.updateScore();
		}
	}

	switchTurns();
}


void GameManager::setCurrentGameState(GameState state)
{
	if (mCurrentState != state)
	{
		mCurrentState = state;
	}
}


GameState GameManager::getCurrentGameState() const
{
	return mCurrentState;
}


void GameManager::setCurrentMoveState(MoveState state)
{
	if (mCurrentMoveState != state)
	{
		mCurrentMoveState = state;
	}
}


MoveState GameManager::getCurrentMoveState() const
{
	return mCurrentMoveState;
}


void GameManager::handleMoveStateChanges(PossibleMove &move)
{
	switch (mCurrentMoveState)
	{
	case (MoveState::NoMove):
	{
		mMovementManager->calculateAllLegalBasicMoves(mCurrentPlayer);
		break;
	}

	case (MoveState::InitiateMove):
	{
		auto possibleMoves = mMovementManager->getMovesForPosition(move.start);
		// delegate possible moves to UI
		break;
	}

	case (MoveState::ExecuteMove):
	{
		executeMove(move);
		checkForEndGameConditions();
		switchTurns();
		break;
	}
	default: break;
	}
}


void GameManager::checkForEndGameConditions()
{
	if (mMovementManager->isCheckmate(mCurrentPlayer))
	{
		setCurrentGameState(GameState::Checkmate);
	}
	else if (mMovementManager->isStalemate(mCurrentPlayer))
	{
		setCurrentGameState(GameState::Stalemate);
	}
	else
	{
		setCurrentGameState(GameState::OnGoing);
	}
}