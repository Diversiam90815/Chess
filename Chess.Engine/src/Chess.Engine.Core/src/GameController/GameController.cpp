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
	mEngine.reset();

	// Set local player based on config
	if (config.mode == GameModeSelection::VsCPU)
	{
		mLocalPlayer			  = config.localPlayer;
		Side			 cpuColor = mConfig.localPlayer == Side::White ? Side::Black : Side::White;

		CPUConfiguration cpuConfig;
		cpuConfig.difficulty		  = static_cast<CPUDifficulty>(mConfig.difficulty);
		cpuConfig.enabled			  = true;
		cpuConfig.cpuColor			  = cpuColor;
		cpuConfig.thinkingTime		  = std::chrono::milliseconds(1000);
		cpuConfig.enableRandomization = true;
		cpuConfig.randomizationFactor = 0.3f;
		cpuConfig.candidateMoveCount  = 7;
	}
	else if (config.mode == GameModeSelection::Multiplayer)
	{
		// TODO: Init Multiplayer Game
	}
	else
	{
		mLocalPlayer = Side::White; // Default for local coop
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


Move GameController::findMove(Square from, Square to, PieceTypes promotion) const
{
	ensureCacheValid();

	for (size_t i = 0; i < mCachedLegalMoves.size(); ++i)
	{
		const Move &m = mCachedLegalMoves[i];

		if (m.from() == from && m.to() == to)
		{
			// if promotion specified, matchit
			if (promotion != PieceTypes::None)
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
