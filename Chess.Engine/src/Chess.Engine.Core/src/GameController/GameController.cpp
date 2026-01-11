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

	switch (config.mode)
	{
	case GameModeSelection::LocalCoop:
	{
		mLocalPlayer = Side::White;
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
		mEngine.setLocalPlayer(spConfig.humanPlayerColor);

		LOG_INFO("Game initialized: Single Player mode (Human: {}, CPU: {}, Difficulty: {})", LoggingHelper::sideToString(spConfig.humanPlayerColor),
				 LoggingHelper::sideToString(cpuColor), LoggingHelper::cpuDifficultyToString(spConfig.aiDifficulty));

		break;
	}
	case GameModeSelection::Multiplayer:
	{
		// TODO: Init Multiplayer Game
		const MultiplayerConfig &mpConfig = config.getMultiplayer();
		mLocalPlayer					  = mpConfig.localPlayerColor;
		mEngine.setLocalPlayer(mpConfig.localPlayerColor);

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
	invalidateCache();
}


void GameController::getLegalMovesFromSquare(Square sq, MoveList &moves)
{
	mEngine.getMovesFromSquare(sq, moves);
}


bool GameController::executeMove(Move move, bool fromRemote)
{
	bool success = mEngine.makeMove(move, fromRemote);

	if (success)
		invalidateCache();

	return success;
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
	return mEngine.getCurrentSide();
}


bool GameController::isLocalPlayerTurn() const
{
	return mEngine.getCurrentSide() == mLocalPlayer;
}


void GameController::switchTurns()
{
	mEngine.switchTurns();
	invalidateCache();
}


bool GameController::isCPUTurn() const
{
	if (mConfig.mode != GameModeSelection::VsCPU)
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
