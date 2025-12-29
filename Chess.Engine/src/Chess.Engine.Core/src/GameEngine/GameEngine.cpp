/*
  ==============================================================================
	Module:         GameEngine
	Description:    Engine of the chess game
  ==============================================================================
*/

#include "GameEngine.h"



void GameEngine::init()
{
	//mChessBoard		= std::make_shared<ChessBoard>();

	//mMoveValidation = std::make_shared<MoveValidation>(mChessBoard);
	//mMoveExecution	= std::make_shared<MoveExecution>(mChessBoard, mMoveValidation);
	//mMoveGeneration = std::make_shared<MoveGeneration>(mChessBoard, mMoveValidation, mMoveExecution);
	//mMoveEvaluation = std::make_shared<MoveEvaluation>(mChessBoard, mMoveGeneration);

	//mCPUPlayer		= std::make_shared<CPUPlayer>(mMoveGeneration, mMoveEvaluation, mChessBoard);

	mWhitePlayer.setPlayerColor(PlayerColor::White);
	mBlackPlayer.setPlayerColor(PlayerColor::Black);
}


void GameEngine::reset()
{
	changeCurrentPlayer(PlayerColor::NoColor);

	mAllMovesForPosition.clear();
	mMovesGeneratedForCurrentTurn = false; // Reset flag
}


void GameEngine::resetGame()
{
	//mChessBoard->removeAllPiecesFromBoard();
	//mChessBoard->initializeBoard();

	//mMoveExecution->clearMoveHistory();

	mWhitePlayer.reset();
	mBlackPlayer.reset();

	reset();
}


PieceType GameEngine::getCurrentPieceTypeAtPosition(const Position position)
{
	//if (!mChessBoard)
	//	return PieceType::DefaultType;

	//auto &chessPiece = mChessBoard->getPiece(position);

	//if (chessPiece)
	//	return chessPiece->getType();

	return PieceType::DefaultType;
}


std::vector<PossibleMove> GameEngine::getPossibleMoveForPosition()
{
	return mAllMovesForPosition;
}

//
//bool GameEngine::getBoardState(BoardStateArray boardState)
//{
//	if (!mChessBoard)
//		return false;
//
//	std::lock_guard<std::mutex> lock(mMutex);
//
//	return mChessBoard->getBoardState(boardState);
//}


void GameEngine::startGame()
{
	reset();

	switchTurns();

	//mChessBoard->initializeBoard(); // Reset the board
}


void GameEngine::changeCurrentPlayer(PlayerColor player)
{
	if (mCurrentPlayer != player)
	{
		mCurrentPlayer = player;

		for (auto &observer : mObservers)
		{
			auto obs = observer.lock();

			if (obs)
				obs->onChangeCurrentPlayer(mCurrentPlayer);
		}
	}
}


PlayerColor GameEngine::getCurrentPlayer() const
{
	return mCurrentPlayer;
}


EndGameState GameEngine::checkForEndGameConditions()
{
	const Move *lastMove = mMoveExecution.getLastMove();

	if (lastMove)
	{
		bool isCheckMate = (lastMove->type & MoveType::Checkmate) == MoveType::Checkmate;
		if (isCheckMate)
		{
			LOG_INFO("Detected a Checkmate!");

			auto winner = getWinner();
			if (winner.has_value())
				endGame(EndGameState::Checkmate, winner.value());
			else
				endGame(EndGameState::Checkmate);

			return EndGameState::Checkmate;
		}

		mMoveGeneration.calculateAllLegalBasicMoves(getCurrentPlayer()); // Calculate all legal moves to check if we have a stalemate (no valid moves left)
		bool isStaleMate = mMoveValidation.isStalemate(getCurrentPlayer());
		if (isStaleMate)
		{
			LOG_INFO("Detected a Stalemate");

			auto winner = getWinner();
			if (winner.has_value())
				endGame(EndGameState::StaleMate, winner.value());
			else
				endGame(EndGameState::StaleMate);

			return EndGameState::StaleMate;
		}

		LOG_INFO("Game is still on-going. We switch player's turns!");
		return EndGameState::OnGoing;
	}

	LOG_WARNING("Couldn't find the last move! Game is still on-going");
	return EndGameState::OnGoing;
}


void GameEngine::setLocalPlayer(PlayerColor player)
{
	if (player == PlayerColor::White)
	{
		LOG_INFO("We start as white player");

		mWhitePlayer.setIsLocalPlayer(true);
		mBlackPlayer.setIsLocalPlayer(false);
	}
	else if (player == PlayerColor::Black)
	{
		LOG_INFO("We start as black player!");

		mWhitePlayer.setIsLocalPlayer(false);
		mBlackPlayer.setIsLocalPlayer(true);
	}
}


PlayerColor GameEngine::getLocalPlayer() const
{
	if (mWhitePlayer.isLocalPlayer())
		return PlayerColor::White;
	else if (mBlackPlayer.isLocalPlayer())
		return PlayerColor::Black;
	else
		return PlayerColor::NoColor;
}


void GameEngine::switchTurns()
{
	mMovesGeneratedForCurrentTurn = false;			// Reset flag for the new turn

	if (getCurrentPlayer() == PlayerColor::NoColor) // We are in init state and set the first round's player : white
	{
		LOG_INFO("Since we setup the game now, we select the white player as the current player!");
		changeCurrentPlayer(PlayerColor::White);
		return;
	}

	if (getCurrentPlayer() == PlayerColor::White)
	{
		changeCurrentPlayer(PlayerColor::Black);
		LOG_INFO("Current player is {}", LoggingHelper::playerColourToString(getCurrentPlayer()).c_str());

		return;
	}

	changeCurrentPlayer(PlayerColor::White);

	LOG_INFO("Current player is {}", LoggingHelper::playerColourToString(getCurrentPlayer()).c_str());
}


bool GameEngine::calculateAllMovesForPlayer()
{
	if (!mMovesGeneratedForCurrentTurn)
	{
		LOG_INFO("We start calculating this player's possible moves!");
		bool result					  = mMoveGeneration.calculateAllLegalBasicMoves(getCurrentPlayer());
		mMovesGeneratedForCurrentTurn = true;
		return result;
	}
	return false;
}


bool GameEngine::initiateMove(const Position &startPosition)
{
	LOG_INFO("We started to initate a move with starting position {}", LoggingHelper::positionToString(startPosition).c_str());

	mAllMovesForPosition.clear();

	auto possibleMoves = mMoveGeneration.getMovesForPosition(startPosition);

	mAllMovesForPosition.reserve(possibleMoves.size());
	mAllMovesForPosition = possibleMoves;

	LOG_INFO("Number of possible moves for the current position is {}", mAllMovesForPosition.size());
	return !mAllMovesForPosition.empty();
}


void GameEngine::executeMove(PossibleMove &tmpMove, bool fromRemote)
{
	PossibleMove moveToExecute{};

	if (fromRemote)
	{
		// Initiate move first to get all moves for current position
		Position startPosition = tmpMove.start;
		initiateMove(startPosition);
	}

	for (auto &move : mAllMovesForPosition)
	{
		if (move == tmpMove)
		{
			moveToExecute = move;

			// On pawn promotion, assign the promotion piece type
			if ((moveToExecute.type & MoveType::PawnPromotion) == MoveType::PawnPromotion)
			{
				moveToExecute.promotionPiece = tmpMove.promotionPiece;
			}
			break;
		}
	}

	std::lock_guard<std::mutex> lock(mMutex);
	Move						executedMove = mMoveExecution.executeMove(moveToExecute, fromRemote);

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

	checkForEndGameConditions();
}


void GameEngine::undoMove()
{
	const Move *lastMove = mMoveExecution.getLastMove();

	if (!lastMove)
	{
		LOG_WARNING("No moves found to undo!");
		return;
	}

	//mChessBoard->movePiece(lastMove->endingPosition, lastMove->startingPosition);

	if (lastMove->capturedPiece != PieceType::DefaultType)
	{
		PlayerColor capturedColor  = (lastMove->player == PlayerColor::White) ? PlayerColor::Black : PlayerColor::White;
		//auto		pieceToRestore = ChessPiece::CreatePiece(lastMove->capturedPiece, capturedColor);

		//if (pieceToRestore)
		//{
		//	mChessBoard->setPiece(lastMove->endingPosition, pieceToRestore);
		//}

		if (lastMove->player == PlayerColor::White)
		{
			mWhitePlayer.removeLastCapturedPiece();
		}
		else if (lastMove->player == PlayerColor::Black)
		{
			mBlackPlayer.removeLastCapturedPiece();
		}
	}/*

	auto &piece = mChessBoard->getPiece(lastMove->startingPosition);

	if (piece)
		piece->decreaseMoveCounter();

	*/
	mMoveExecution.removeLastMove();
}


void GameEngine::endGame(EndGameState state, PlayerColor player)
{
	for (auto &observer : mObservers)
	{
		auto obs = observer.lock();

		if (obs)
			obs->onEndGame(state, player);
	}
}


std::optional<PlayerColor> GameEngine::getWinner() const
{
	const Move *lastMove = mMoveExecution.getLastMove();

	if (!lastMove)
		return std::nullopt;

	// Check if the last move resulted in a checkmate
	bool isCheckMate = (lastMove->type & MoveType::Checkmate) == MoveType::Checkmate;

	if (isCheckMate)
		return lastMove->player;

	return std::nullopt;
}


bool GameEngine::checkForValidMoves(const PossibleMove &move)
{
	if (move.start == move.end)						// The user aborted the move by clicking the piece again
		return false;

	for (auto &possibleMove : mAllMovesForPosition) // Check if the move is a valid move
	{
		if (move == possibleMove)
			return true;
	}

	return false;
}


bool GameEngine::checkForPawnPromotionMove(const PossibleMove &move)
{
	for (auto &possibleMove : mAllMovesForPosition)
	{
		if (move == possibleMove)
			return (possibleMove.type & MoveType::PawnPromotion) == MoveType::PawnPromotion;
	}
	return false;
}


void GameEngine::setCPUConfiguration(const CPUConfiguration &config)
{
	//if (mCPUPlayer)
		mCPUPlayer.setCPUConfiguration(config);
}


CPUConfiguration GameEngine::getCPUConfiguration() const
{
	//if (mCPUPlayer)
		return mCPUPlayer.getCPUConfiguration();

	//return {};
}


bool GameEngine::isCPUPlayer(PlayerColor player) const
{
	return /*mCPUPlayer &&*/ mCPUPlayer.isCPUPlayer(player);
}


void GameEngine::requestCPUMoveAsync()
{
	//if (mCPUPlayer)
		return mCPUPlayer.requestMoveAsync();
}
