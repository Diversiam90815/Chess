/*
  ==============================================================================

	Class:          GameManager

	Description:    Manager for the Chess game

  ==============================================================================
*/

#include "GameManager.h"
// #include <strsafe.h>


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
	mLog.initLogging();

	mMovementManager = std::make_unique<MovementManager>();

	if (mDelegate)
	{
		mMovementManager->setDelegate(mDelegate);
	}

	mMovementManager->init();

	mWhitePlayer.setPlayerColor(PlayerColor::White);
	mBlackPlayer.setPlayerColor(PlayerColor::Black);

	//clearState();
}


void GameManager::startGame()
{
	clearState();

	mMovementManager->mChessBoard->initializeBoard(); // Reset the board
	setCurrentPlayer(PlayerColor::White);			  // Setting the player at the end, since this will trigger the move calculation
}


void GameManager::clearState()
{
	setCurrentPlayer(PlayerColor::NoColor);

	setCurrentGameState(GameState::Init);
	setCurrentMoveState(MoveState::NoMove);
	mAllMovesForPosition.clear();
	mMovesGeneratedForCurrentTurn = false; // Reset flag
}


void GameManager::setDelegate(PFN_CALLBACK pDelegate)
{
	mDelegate = pDelegate;
	mWhitePlayer.setDelegate(pDelegate);
	mBlackPlayer.setDelegate(pDelegate);
	if (mMovementManager)
	{
		mMovementManager->setDelegate(pDelegate);
	}
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


std::vector<PossibleMove> GameManager::getPossibleMoveForPosition()
{
	return mAllMovesForPosition;
}


bool GameManager::getBoardState(int boardState[BOARD_SIZE][BOARD_SIZE])
{
	if (!mMovementManager || !mMovementManager->mChessBoard)
		return false;

	auto &board = mMovementManager->mChessBoard;

	for (int y = 0; y < BOARD_SIZE; ++y)
	{
		for (int x = 0; x < BOARD_SIZE; ++x)
		{
			Position pos	  = {x, y};
			auto	&piece	  = board->getPiece(pos);

			int		 colorVal = 0; // 0 = no color
			int		 typeVal  = 0; // 0 = PieceType::DefaultType

			if (piece)
			{
				colorVal = static_cast<int>(piece->getColor()) & 0xF;
				typeVal	 = static_cast<int>(piece->getType()) & 0xF;
			}

			// Pack color in high nibble, type in low nibble:
			// bits [4..7] = color, bits [0..3] = piece type
			int encoded		 = (colorVal << 4) | (typeVal & 0xF);

			boardState[y][x] = encoded;
		}
	}
	return true;
}


void GameManager::switchTurns()
{
	setCurrentMoveState(MoveState::NoMove);
	mMovesGeneratedForCurrentTurn = false; // Reset flag for the new turn

	if (getCurrentPlayer() == PlayerColor::White)
	{
		setCurrentPlayer(PlayerColor::Black);
		LOG_INFO("Current player is {}", LoggingHelper::playerColourToString(getCurrentPlayer()).c_str());

		return;
	}

	setCurrentPlayer(PlayerColor::White);

	LOG_INFO("Current player is {}", LoggingHelper::playerColourToString(getCurrentPlayer()).c_str());
}


void GameManager::executeMove(PossibleMove &move)
{
	Move executedMove = mMovementManager->executeMove(move);

	LoggingHelper::logMove(executedMove);

	if (executedMove.capturedPiece != PieceType::DefaultType)
	{
		if (getCurrentPlayer() == PlayerColor::White)
		{
			mWhitePlayer.addCapturedPiece(executedMove.capturedPiece);
			mWhitePlayer.updateScore();
		}
		else
		{
			mBlackPlayer.addCapturedPiece(executedMove.capturedPiece);
			mBlackPlayer.updateScore();
		}
	}

	if (mDelegate)
	{
		mDelegate(delegateMessage::moveExecuted, 0);
	}

	checkForEndGameConditions();
}


void GameManager::undoMove()
{
	const Move *lastMove = mMovementManager->getLastMove();
	if (!lastMove)
	{
		LOG_WARNING("No moves found to undo!");
		return;
	}

	mMovementManager->mChessBoard->movePiece(lastMove->endingPosition, lastMove->startingPosition);

	if (lastMove->capturedPiece != PieceType::DefaultType)
	{
		PlayerColor capturedColor  = (lastMove->player == PlayerColor::White) ? PlayerColor::Black : PlayerColor::White;
		auto		pieceToRestore = ChessPiece::CreatePiece(lastMove->capturedPiece, capturedColor);

		if (pieceToRestore)
		{
			mMovementManager->mChessBoard->setPiece(lastMove->endingPosition, pieceToRestore);
		}

		if (lastMove->player == PlayerColor::White)
		{
			mWhitePlayer.removeLastCapturedPiece();
		}
		else if (lastMove->player == PlayerColor::Black)
		{
			mBlackPlayer.removeLastCapturedPiece();
		}
	}

	auto &piece = mMovementManager->mChessBoard->getPiece(lastMove->startingPosition);
	piece->decreaseMoveCounter();

	mMovementManager->removeLastMove();
	switchTurns();
}


void GameManager::setCurrentGameState(GameState state)
{
	if (mCurrentState != state)
	{
		mCurrentState = state;
		if (mDelegate)
		{
			mDelegate(delegateMessage::gameStateChanged, &mCurrentState);
		}
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
	if (winner.has_value() && mDelegate)
	{
		mDelegate(delegateMessage::playerHasWon, &winner);
	}
}


std::optional<PlayerColor> GameManager::getWinner() const
{
	if (mCurrentState == GameState::Checkmate)
		return getCurrentPlayer() == PlayerColor::White ? PlayerColor::White : PlayerColor::Black;

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
		if (!mMovesGeneratedForCurrentTurn)
		{
			LOG_INFO("Move State is NoMove -> We start calculating this player's possible moves!");
			mMovementManager->calculateAllLegalBasicMoves(getCurrentPlayer());
			mMovesGeneratedForCurrentTurn = true;
		}
		break;
	}

	case (MoveState::InitiateMove):
	{
		LOG_INFO("We started to initate a move with starting position {}", LoggingHelper::positionToString(move.start).c_str());

		mAllMovesForPosition.clear();

		auto possibleMoves = mMovementManager->getMovesForPosition(move.start);

		mAllMovesForPosition.reserve(possibleMoves.size());
		mAllMovesForPosition = possibleMoves;

		if (mDelegate)
		{
			mDelegate(delegateMessage::initiateMove, 0); // UI can now get all the moves for the piece
		}

		LOG_INFO("Number of possible moves for the current position is {}", mAllMovesForPosition.size());

		break;
	}

	case (MoveState::ExecuteMove):
	{
		LOG_INFO("Executing the move now!");
		executeMove(move);
		break;
	}
	default: break;
	}
}


void GameManager::setCurrentPlayer(PlayerColor player)
{
	if (mCurrentPlayer != player)
	{
		mCurrentPlayer = player;

		if (mDelegate)
		{
			int currentPlayer = (int)mCurrentPlayer;
			mDelegate(delegateMessage::playerChanged, &currentPlayer);
		}
	}
}


PlayerColor GameManager::getCurrentPlayer() const
{
	return mCurrentPlayer;
}


void GameManager::checkForEndGameConditions()
{
	const Move *lastMove = mMovementManager->getLastMove();

	if (lastMove)
	{
		bool isCheckMate = (lastMove->type & MoveType::Checkmate) == MoveType::Checkmate;
		if (isCheckMate)
		{
			LOG_INFO("Detected a Checkmate!");
			setCurrentGameState(GameState::Checkmate);
			endGame();
			return;
		}

		bool isStaleMate = mMovementManager->isStalemate(getCurrentPlayer());
		if (isStaleMate)
		{
			LOG_INFO("Detected a Stalemate");
			setCurrentGameState(GameState::Stalemate);
			endGame();
			return;
		}

		LOG_INFO("Game is still on-going. We switch player's turns!");
		setCurrentGameState(GameState::OnGoing);
		switchTurns();
		return;
	}

	LOG_WARNING("Couldn't find the last move! Game is still on-going");
	setCurrentGameState(GameState::OnGoing);
}
