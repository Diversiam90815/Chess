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


GameManager *GameManager::GetInstance()
{
	static GameManager *sInstance = nullptr;
	if (nullptr == sInstance)
	{
		sInstance = new GameManager();
	}
	return sInstance;
}


void GameManager::ReleaseInstance()
{
	GameManager *sInstance = GetInstance();
	if (sInstance)
	{
		delete sInstance;
	}
}


void GameManager::init()
{
	mMovementManager = std::make_unique<MovementManager>();
	mMovementManager->init();

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


void GameManager::setDelegate(PFN_CALLBACK pDelegate)
{
	mDelegate = pDelegate;
	// Set further Delegates if needed
}


PieceType GameManager::getCurrentPieceTypeAtPosition(const Position position)
{
	if (!mMovementManager)
		return PieceType::DefaultType;

	auto &chessPiece = mMovementManager->mChessBoard->getPiece(position);

	if (chessPiece)
	{
		return chessPiece->getType();
	}
	return PieceType::DefaultType;
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
		break;
	}
	default: break;
	}
}


void GameManager::checkForEndGameConditions()
{
	const Move *lastMove = mMovementManager->getLastMove();

	if (lastMove)
	{
		bool isCheckMate = (lastMove->type & MoveType::Checkmate) == MoveType::Checkmate;
		if (isCheckMate)
		{
			setCurrentGameState(GameState::Checkmate);
			endGame();
			return;
		}

		bool isStaleMate = mMovementManager->isStalemate(mCurrentPlayer);
		if (isStaleMate)
		{
			setCurrentGameState(GameState::Stalemate);
			endGame();
			return;
		}

		setCurrentGameState(GameState::OnGoing);
		switchTurns();
	}
	setCurrentGameState(GameState::OnGoing);
}