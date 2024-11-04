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

	clearState();
}


void GameManager::clearState()
{
	mCurrentPlayer = PlayerColor::White;
	mWhitePlayer.setOnTurn(true);

	setCurrentGameState(GameState::Init);
	setCurrentMoveState(MoveState::NoMove);
	mAllMovesForPosition.clear();
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

	// switchTurns();
	checkForEndGameConditions();
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


void GameManager::resetGame()
{
	mMovementManager->mChessBoard->removeAllPiecesFromBoard();
	mMovementManager->mChessBoard->initializeBoard();

	mWhitePlayer.reset();
	mBlackPlayer.reset();

	clearState();
}


void GameManager::endGame() const
{
	auto winner = getWinner();
	if (winner.has_value())
	{
		// winner is Winner (set delegate to UI)
	}
}


std::optional<PlayerColor> GameManager::getWinner() const
{
	if (mCurrentState == GameState::Checkmate)
		return mCurrentPlayer == PlayerColor::White ? PlayerColor::White : PlayerColor::Black;
	else if (mCurrentState == GameState::Stalemate)
		return std::nullopt; // Draw in case of stalemate
	return std::nullopt;
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
		mAllMovesForPosition.clear();

		auto possibleMoves = mMovementManager->getMovesForPosition(move.start);

		mAllMovesForPosition.reserve(possibleMoves.size());
		mAllMovesForPosition = possibleMoves;
		// delegate possible moves to UI
		break;
	}

	case (MoveState::ExecuteMove):
	{
		executeMove(move);
		// checkForEndGameConditions();
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
		endGame();
	}
	else if (mMovementManager->isStalemate(mCurrentPlayer))
	{
		setCurrentGameState(GameState::Stalemate);
		endGame();
	}
	else
	{
		setCurrentGameState(GameState::OnGoing);
		switchTurns();
	}
}