/*
  ==============================================================================
	Module:         GameController
	Description:    IGameController implementation
  ==============================================================================
*/

#include "GameController.h"


GameController::GameController() : mCPUPlayer(mEngine) {}


bool GameController::initializeGame(GameConfiguration config)
{
	mConfig = config;

	mEngine.init();
	mEngine.resetGame();

	mWhitePlayer.setPlayerColor(Side::White);
	mBlackPlayer.setPlayerColor(Side::Black);

	switch (config.mode)
	{
	case GameModeSelection::LocalCoop:
	{
		mLocalPlayer   = Side::White;
		mCurrentPlayer = Side::White;

		LOG_INFO("Game initialized: Local coop mode!");
		break;
	}
	case GameModeSelection::SinglePlayer:
	{
		const SinglePlayerConfig &spConfig = config.getSinglePlayer();
		mLocalPlayer					   = spConfig.humanPlayerColor;
		Side			 cpuColor		   = (spConfig.humanPlayerColor == Side::White) ? Side::Black : Side::White;

		CPUConfiguration cpuConfig;
		cpuConfig.difficulty		  = spConfig.aiDifficulty;
		cpuConfig.enabled			  = true;
		cpuConfig.cpuColor			  = cpuColor;
		cpuConfig.enableRandomization = true;

		mCPUPlayer.configure(cpuConfig);

		LOG_INFO("Game initialized: Single Player mode (Human: {}, CPU: {}, Difficulty: {})", LoggingHelper::sideToString(spConfig.humanPlayerColor),
				 LoggingHelper::sideToString(cpuColor), LoggingHelper::cpuDifficultyToString(spConfig.aiDifficulty));

		break;
	}
	case GameModeSelection::Multiplayer:
	{
		// TODO: Init Multiplayer Game
		const MultiplayerConfig &mpConfig = config.getMultiplayer();
		mLocalPlayer					  = mpConfig.localPlayerColor;

		LOG_INFO("Game initialized: Multiplayer mode (Local player: {})", LoggingHelper::sideToString(mpConfig.localPlayerColor));

		break;
	}
	}

	invalidateCache();

	LOG_INFO("Game initialized successfully!");
	return true;
}


void GameController::resetGame()
{
	mEngine.resetGame();

	mWhitePlayer.reset();
	mBlackPlayer.reset();
	mCurrentPlayer = Side::None;

	invalidateCache();
}


void GameController::getLegalMovesFromSquare(Square sq, MoveList &moves)
{
	mEngine.getMovesFromSquare(sq, moves);

	if (moves.size() > 0)
	{
		mCachedLegalMoves = moves;
		mCacheValid		  = true;
	}
}


MoveExecutionResult GameController::executeMove(Move move)
{
	MoveExecutionResult result = mEngine.makeMove(move);

	if (result)
		invalidateCache();

	if (result.capturedPiece != PieceType::None)
	{
		Side currentSide = getCurrentSide();

		if (currentSide == Side::White)
			mWhitePlayer.addCapturedPiece(result.capturedPiece);
		else if (currentSide == Side::Black)
			mBlackPlayer.addCapturedPiece(result.capturedPiece);
	}

	return result;
}


bool GameController::undoLastMove()
{
	bool success = mEngine.undoMove();

	if (success)
		invalidateCache();

	return success;
}


bool GameController::isPromotionMove(Square from, Square to) const
{
	ensureCacheValid();

	for (size_t i = 0; i < mCachedLegalMoves.size(); ++i)
	{
		const Move &m = mCachedLegalMoves[i];
		if (m.from() == from && m.to() == to && m.isPromotion())
			return true;
	}

	return false;
}


Move GameController::findMove(Square from, Square to, PieceType promotion) const
{
	ensureCacheValid();

	for (size_t i = 0; i < mCachedLegalMoves.size(); ++i)
	{
		const Move &m = mCachedLegalMoves[i];

		if (m.from() == from && m.to() == to)
		{
			// if promotion specified, matchit
			if (promotion != PieceType::None)
			{
				if (m.isPromotion() && m.promotionPieceOffset() == promotion)
					return m;
			}
			else if (!m.isPromotion())
				return m;
		}
	}

	return Move();
}


EndGameState GameController::checkEndGame()
{
	return mEngine.checkForEndGameConditions();
}


Side GameController::getCurrentSide() const
{
	return mCurrentPlayer;
}


bool GameController::isLocalPlayerTurn() const
{
	return mCurrentPlayer == mLocalPlayer;
}


void GameController::switchTurns()
{
	if (mCurrentPlayer == Side::None)
	{
		changeCurrentPlayer(Side::White);
		return;
	}

	Side next = (mCurrentPlayer == Side::White) ? Side::Black : Side::White;

	changeCurrentPlayer(next);

	invalidateCache();
}


void GameController::changeCurrentPlayer(Side player)
{
	if (mCurrentPlayer == player)
		return;

	mCurrentPlayer = player;
}


bool GameController::isCPUTurn() const
{
	if (mConfig.mode != GameModeSelection::SinglePlayer)
		return false;

	return !isLocalPlayerTurn();
}


void GameController::requestCPUMoveAsync()
{
	mCPUPlayer.calculateMoveAsync(
		[this](Move move)
		{
			if (mOnCPUMove)
				mOnCPUMove(move);
		});
}


void GameController::cancelCPUCalculation()
{
	// TODO
}


void GameController::setCPUMoveCallback(std::function<void(Move)> callback)
{
	mOnCPUMove = std::move(callback);
}


void GameController::ensureCacheValid() const
{
	if (!mCacheValid)
	{
		mCachedLegalMoves.clear();
		const_cast<GameEngine &>(mEngine).generateLegalMoves(mCachedLegalMoves);
		mCacheValid = true;
	}
}
