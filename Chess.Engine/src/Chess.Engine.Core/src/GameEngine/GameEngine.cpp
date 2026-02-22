/*
  ==============================================================================
	Module:         GameEngine
	Description:    Engine of the chess game
  ==============================================================================
*/

#include "GameEngine.h"


GameEngine::GameEngine() : mMoveGeneration(mChessBoard), mMoveExecution(mChessBoard), mMoveValidation(mChessBoard, mMoveGeneration, mMoveExecution) {}


void GameEngine::init()
{
	mChessBoard.init();
}


void GameEngine::startGame()
{
	resetGame();
	LOG_INFO("Game started..");
}


void GameEngine::resetGame()
{
	mChessBoard.init();
	mMoveExecution.clearHistory();
}


MoveExecutionResult GameEngine::makeMove(Move move)
{
	std::lock_guard<std::mutex> lock(mMoveMutex);

	MoveExecutionResult			result;

	result.notation = getMoveNotation(move);

	if (!mMoveValidation.isMoveLegal(move))
	{
		LOG_WARNING("Illegal move attemted: {}", MoveNotation::toUCI(move));
		result.success = false;
		return result;
	}

	if (!mMoveExecution.makeMove(move))
	{
		LOG_ERROR("Move execution failed: {}", MoveNotation::toUCI(move));
		result.success = false;
		return result;
	}

	if (move.isCapture())
		result.capturedPiece = mMoveExecution.getLastCapturedPiece();

	result.success = true;
	return result;
}


bool GameEngine::undoMove()
{
	std::lock_guard<std::mutex> lock(mMoveMutex);

	// Get the captured piece before undoing
	PieceType					capturedPiece = mMoveExecution.getLastCapturedPiece();

	if (!mMoveExecution.unmakeMove())
	{
		LOG_WARNING("No move to undo!");
		return false;
	}

	return true;
}


void GameEngine::generateLegalMoves(MoveList &moves)
{
	mMoveValidation.generateLegalMoves(moves);
}


bool GameEngine::isMoveLegal(Move move)
{
	return mMoveValidation.isMoveLegal(move);
}


void GameEngine::getMovesFromSquare(Square from, MoveList &moves)
{
	MoveList allMoves;
	generateLegalMoves(allMoves);

	moves.clear();
	for (size_t i = 0; i < allMoves.size(); ++i)
	{
		Square fromVariable = allMoves[i].from();
		LOG_DEBUG("FromVariable: {}", to_index(fromVariable));
		LOG_DEBUG("From: {}", to_index(from));

		if (allMoves[i].from() == from)
			moves.push(allMoves[i]);
	}
}


bool GameEngine::isInCheck() const
{
	return mMoveValidation.isInCheck();
}


bool GameEngine::isCheckmate()
{
	return mMoveValidation.isCheckmate();
}


bool GameEngine::isStalemate()
{
	return mMoveValidation.isStalemate();
}


bool GameEngine::isDraw() const
{
	return mMoveValidation.isDraw();
}


EndGameState GameEngine::checkForEndGameConditions()
{
	if (mMoveValidation.isCheckmate())
	{
		LOG_INFO("Checkmate!");
		auto winner = getWinner();
		return EndGameState::Checkmate;
	}

	if (mMoveValidation.isStalemate())
	{
		LOG_INFO("Stalemate!");
		return EndGameState::StaleMate;
	}

	if (mMoveValidation.isDraw())
	{
		LOG_INFO("Draw!");
		return EndGameState::Draw;
	}

	return EndGameState::OnGoing;
}


Side GameEngine::getWinner() const
{
	// Winner is the side that just moved -> opponent got mated
	return (mChessBoard.getCurrentSide() == Side::White) ? Side::Black : Side::White;
}


uint64_t GameEngine::getHash()
{
	return mChessBoard.getHash();
}


const std::vector<MoveHistoryEntry> &GameEngine::getMoveHistory() const
{
	return mMoveExecution.getHistory();
}


std::string GameEngine::getMoveNotation(Move move) const
{
	return MoveNotation::toSAN(move, mChessBoard, false, false);
}
