/*
  ==============================================================================
	Module:			CPUPlayer
	Description:    Managing the CPU opponent player
  ==============================================================================
*/

#include "CPUPlayer.h"


CPUPlayer::CPUPlayer(std::shared_ptr<MoveGeneration> moveGeneration, std::shared_ptr<ChessBoard> board)
	: mMoveGeneration(moveGeneration), mBoard(board), mRandomGenerator(mRandomDevice)
{
}


void CPUPlayer::setCPUConfiguration(const CPUConfiguration &config)
{
	mConfig = config;
	LOG_INFO("CPU player configured:");
	LOG_INFO("\tDifficulty:\t{}", static_cast<int>(config.difficulty));
	LOG_INFO("\tPlayer:\t{}", LoggingHelper::playerColourToString(config.cpuColor).c_str());
	LOG_INFO("\tEnabled:\t{}", LoggingHelper::boolToString(config.enabled).c_str());
}


std::future<PossibleMove> CPUPlayer::getBestMoveAsync(PlayerColor player)
{
	return std::async(std::launch::async, [this, player]() { return getBestMove(player); });
}


PossibleMove CPUPlayer::getBestMove(PlayerColor player)
{
	if (!isCPUPlayer(player))
	{
		LOG_WARNING("getBestMove called for non-CPU player! Called from {}", LoggingHelper::playerColourToString(player).c_str());
		return {};
	}

	// Generate all legal moves
	mMoveGeneration->calculateAllLegalBasicMoves(player);

	// Get all possible moves for all pieces
	auto					  playerPieces = mBoard->getPiecesFromPlayer(player);
	std::vector<PossibleMove> allMoves;

	for (const auto &[position, piece] : playerPieces)
	{
		auto moves = mMoveGeneration->getMovesForPosition(position);
		allMoves.insert(allMoves.end(), moves.begin(), moves.end());
	}

	if (allMoves.empty())
	{
		LOG_WARNING("No legal moves available for CPU player!");
		return {};
	}

	// Simulate thinking
	simulateThinking();

	// Select move based on difficulty (for now we just test random moves)
	switch (mConfig.difficulty)
	{
	case CPUDifficulty::Random: return getRandomMove(allMoves);
	case CPUDifficulty::Easy: return getEasyMove(allMoves);
	case CPUDifficulty::Medium: return getMediumMove(allMoves);
	case CPUDifficulty::Hard: return getHardMove(allMoves);
	default: return getRandomMove(allMoves);
	}
}

bool CPUPlayer::isCPUPlayer(PlayerColor player) const
{
	return mConfig.enabled && (player == mConfig.cpuColor);
}


PossibleMove CPUPlayer::getRandomMove(const std::vector<PossibleMove> &moves)
{
	std::uniform_int_distribution<size_t> distribution(0, moves.size() - 1);
	size_t								  randomIndex = distribution(mRandomGenerator);

	LOG_INFO("CPU selected random move {}/{}", randomIndex + 1, moves.size());
	return moves[randomIndex];
}


PossibleMove CPUPlayer::getEasyMove(const std::vector<PossibleMove> &moves)
{
	// TODO
	return PossibleMove();
}


PossibleMove CPUPlayer::getMediumMove(const std::vector<PossibleMove> &moves)
{
	// TODO
	return PossibleMove();
}


PossibleMove CPUPlayer::getHardMove(const std::vector<PossibleMove> &moves)
{
	// TODO
	return PossibleMove();
}


void CPUPlayer::simulateThinking()
{
	if (mConfig.thinkingTime.count() > 0)
	{
		std::this_thread::sleep_for(mConfig.thinkingTime);
	}
}
