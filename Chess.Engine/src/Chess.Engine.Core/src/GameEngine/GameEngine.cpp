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
	mWhitePlayer.setPlayerColor(Side::White);
	mBlackPlayer.setPlayerColor(Side::Black);
}


void GameEngine::startGame()
{
	resetGame();
	switchTurns();
	LOG_INFO("Game started..");
}


void GameEngine::resetGame()
{
	mChessBoard.init();
	mMoveExecution.clearHistory();
	mWhitePlayer.reset();
	mBlackPlayer.reset();
	mCurrentPlayer = Side::None;
}


bool GameEngine::makeMove(Move move, bool fromRemote)
{
	std::lock_guard<std::mutex> lock(mMoveMutex);

	if (!mMoveValidation.isMoveLegal(move))
	{
		LOG_WARNING("Illegal move attemted: {}", mMoveNotation.toUCI(move));
		return false;
	}

	if (!mMoveExecution.makeMove(move))
	{
		LOG_ERROR("Move execution failed: {}", mMoveNotation.toUCI(move));
		return false;
	}

	// generate notation
	bool		inCheck	 = mMoveValidation.isInCheck();
	bool		isMate	 = inCheck && mMoveValidation.isCheckmate();
	std::string notation = mMoveNotation.toSAN(move, mChessBoard, inCheck, isMate);
	LOG_INFO("Move: {}", notation);

	// notify observers
	notifyMoveExecuted(move, fromRemote);

	checkForEndGameConditions();

	return true;
}


bool GameEngine::undoMove()
{
	std::lock_guard<std::mutex> lock(mMoveMutex);

	if (!mMoveExecution.unmakeMove())
	{
		LOG_WARNING("No move to undo!");
		return false;
	}

	LOG_INFO("Move undone");

	notifyMoveUndone();

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
		endGame(EndGameState::Checkmate, winner);
		return EndGameState::Checkmate;
	}

	if (mMoveValidation.isStalemate())
	{
		LOG_INFO("Stalemate!");
		endGame(EndGameState::StaleMate);
		return EndGameState::StaleMate;
	}

	if (mMoveValidation.isDraw())
	{
		LOG_INFO("Draw!");
		endGame(EndGameState::Draw);
		return EndGameState::Draw;
	}

	return EndGameState::OnGoing;
}


void GameEngine::switchTurns()
{
	if (mCurrentPlayer == Side::None)
	{
		changeCurrentPlayer(Side::White);
		return;
	}

	Side next = (mCurrentPlayer == Side::White) ? Side::Black : Side::White;

	changeCurrentPlayer(next);
}


void GameEngine::changeCurrentPlayer(Side player)
{
	if (mCurrentPlayer == player)
		return;

	mCurrentPlayer = player;

	for (auto &observer : mObservers)
	{
		if (auto obs = observer.lock())
			obs->onChangeCurrentPlayer(mCurrentPlayer);
	}
}


void GameEngine::setLocalPlayer(Side player)
{
	mWhitePlayer.setIsLocalPlayer(player == Side::White);
	mBlackPlayer.setIsLocalPlayer(player == Side::Black);
}


Side GameEngine::getLocalPlayer() const
{
	if (mWhitePlayer.isLocalPlayer())
		return Side::White;

	if (mBlackPlayer.isLocalPlayer())
		return Side::Black;

	return Side::None;
}


void GameEngine::endGame(EndGameState state, Side player)
{
	for (auto &observer : mObservers)
	{
		if (auto obs = observer.lock())
			obs->onEndGame(state, player);
	}
}


Side GameEngine::getWinner() const
{
	// Winner is the side that just moved -> opponent got mated
	return (mChessBoard.getCurrentSide() == Side::White) ? Side::Black : Side::White;
}


uint64_t GameEngine::getHash()
{
	return 0;
}


const std::vector<MoveHistoryEntry> &GameEngine::getMoveHistory() const
{
	return mMoveExecution.getHistory();
}


std::string GameEngine::getMoveNotation(Move move) const
{
	return mMoveNotation.toSAN(move, mChessBoard, false, false);
}


void GameEngine::notifyMoveExecuted(Move move, bool fromRemote)
{
	for (auto &observer : mObservers)
	{
		if (auto obs = observer.lock())
			obs->onMoveExecuted(move, fromRemote);
	}
}


void GameEngine::notifyMoveUndone()
{
	for (auto &observer : mObservers)
	{
		if (auto obs = observer.lock())
			obs->onMoveUndone();
	}
}
